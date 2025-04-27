#include "CubeNode.hpp"
#include "cube.h"

#include <QDebug>
#include <QFile>
#include <QMatrix4x4>
#include <QPainter>
#include <QTextStream>
#include <array>

#include "construct/RhiContext.hpp"
#include "construct/Scene.hpp"
#include "construct/Utils.hpp"

static const auto TEXTURE_SIZE = QSize(512, 512);

void CubeNode::initialize(RhiContext &context)
{
	qDebug() << "CubeNode::initialize(" << context << ")";
	m_pipeline.reset();
	
	m_vbuf.reset(context.m_rhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer, sizeof(cube)));
	m_vbuf->create();
	
	// Create two uniform buffers:
	// Vertex UBO: 2 matrices = 32 floats (128 bytes)
	m_vertexUbuf.reset(context.m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, 128));
	m_vertexUbuf->create();
	// Fragment UBO: 28 floats (112 bytes)
	m_fragmentUbuf.reset(context.m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, 112));
	m_fragmentUbuf->create();
	
	m_texture.reset(context.m_rhi->newTexture(QRhiTexture::RGBA8, TEXTURE_SIZE));
	m_texture->create();
	qDebug()<< "TEXTURE" << m_texture->pixelSize() << "x" << m_texture->depth();
	
	m_sampler.reset(context.m_rhi->newSampler(QRhiSampler::Linear, QRhiSampler::Linear,
											  QRhiSampler::None, QRhiSampler::Repeat, QRhiSampler::Repeat));
	m_sampler->create();
	
	m_srb.reset(context.m_rhi->newShaderResourceBindings());
	m_srb->setBindings({
		// Binding 0: Vertex UBO
		QRhiShaderResourceBinding::uniformBuffer(0, QRhiShaderResourceBinding::VertexStage, m_vertexUbuf.get()),
		// Binding 1: Fragment UBO
		QRhiShaderResourceBinding::uniformBuffer(1, QRhiShaderResourceBinding::FragmentStage, m_fragmentUbuf.get()),
		QRhiShaderResourceBinding::sampledTexture(2, QRhiShaderResourceBinding::FragmentStage, m_texture.get(), m_sampler.get())
	});
	m_srb->create();
	
	// Setup pipeline
	m_pipeline.reset(context.m_rhi->newGraphicsPipeline());
	m_pipeline->setDepthTest(true);
	m_pipeline->setDepthWrite(true);
	m_pipeline->setCullMode(QRhiGraphicsPipeline::None);
	m_pipeline->setShaderStages({
		{ QRhiShaderStage::Vertex, getShader(QLatin1String(":/cube.vert.qsb")) },
		{ QRhiShaderStage::Fragment, getShader(QLatin1String(":/cube.frag.qsb")) }
	});
	
	QRhiVertexInputLayout inputLayout;
	inputLayout.setBindings({ { 8 * sizeof(float) } }); // 3 + 2 + 3 = 8 floats per vertex
	inputLayout.setAttributes({
		{ 0, 0, QRhiVertexInputAttribute::Float3, 0 },                   // Position at offset 0
		{ 0, 1, QRhiVertexInputAttribute::Float2, 3 * sizeof(float) },    // UVs at offset 3 floats
		{ 0, 2, QRhiVertexInputAttribute::Float3, 5 * sizeof(float) }     // Normals at offset 5 floats
	});
	m_pipeline->setSampleCount(context.m_sampleCount);
	m_pipeline->setVertexInputLayout(inputLayout);
	m_pipeline->setShaderResourceBindings(m_srb.get());
	m_pipeline->setRenderPassDescriptor(context.renderPassDescriptor);
	m_pipeline->create();
	
	auto resourceUpdates = context.m_rhi->nextResourceUpdateBatch();
	resourceUpdates->uploadStaticBuffer(m_vbuf.get(), cube);
	resourceUpdates->uploadTexture(m_texture.get(), dummyTexture(TEXTURE_SIZE, "CUBE"));
	context.cb->resourceUpdate(resourceUpdates);
}

void CubeNode::update(RhiContext &context, qint64 interval)
{
	// Update rotation angle.
	m_angle = fmod(m_angle + (interval / 1000.0), (M_PI * 2.0));
	const auto rotation = cos(m_angle) * 90;
	
	// Get camera matrices from the Scene.
	QMatrix4x4 viewMatrix = mScene->camera()->viewMatrix();
	QMatrix4x4 projectionMatrix = mScene->camera()->projectionMatrix();
	
	// Compute a model matrix for rotation.
	QMatrix4x4 modelMatrix;
	modelMatrix.rotate(rotation, 0, 1, 0);
	
	// Compute a full model transform that includes the rotation.
	QMatrix4x4 fullModel = modelMatrix * m_modelMatrix;
	// Then compute the final MVP using the full world transform.
	QMatrix4x4 mvp = projectionMatrix * viewMatrix * fullModel;

	// --- Update Vertex UBO ---
	// Vertex UBO layout: [ mvp (16 floats) | model (16 floats) ]
	std::array<float, 32> vertexUniforms;
	const float *mvpData = mvp.constData();
	for (int i = 0; i < 16; ++i)
		vertexUniforms[i] = mvpData[i];
	// Use the node's local model matrix (or another matrix as needed)
	const float *modelData = fullModel.constData();
	for (int i = 0; i < 16; ++i)
		vertexUniforms[16 + i] = modelData[i];
	context.ensureResourceUpdates();
	context.resourceUpdates->updateDynamicBuffer(m_vertexUbuf.get(), 0, vertexUniforms.size() * sizeof(float), vertexUniforms.data());
	
	// --- Update Fragment UBO ---
	// Fragment UBO layout: [ mvp (16 floats) | lightDir (vec3 + pad) | cameraPos (vec3 + pad) | objectColor (vec3 + pad) ]
	std::array<float, 28> fragmentUniforms;
	for (int i = 0; i < 16; ++i)
		fragmentUniforms[i] = mvpData[i];
	
	// Light direction (fixed for now)
	QVector3D lightDir = QVector3D(0.5f, -1.0f, -0.5f).normalized();
	fragmentUniforms[16] = lightDir.x();
	fragmentUniforms[17] = lightDir.y();
	fragmentUniforms[18] = lightDir.z();
	fragmentUniforms[19] = 0.0f; // padding
	
	// Camera position.
	QVector3D cameraPos = mScene->camera()->position();
	fragmentUniforms[20] = cameraPos.x();
	fragmentUniforms[21] = cameraPos.y();
	fragmentUniforms[22] = cameraPos.z();
	fragmentUniforms[23] = 0.0f; // padding
	
	// Object color (default light gray).
	QVector3D objectColor(0.8f, 0.8f, 0.8f);
	fragmentUniforms[24] = objectColor.x();
	fragmentUniforms[25] = objectColor.y();
	fragmentUniforms[26] = objectColor.z();
	fragmentUniforms[27] = 0.0f; // padding
	
	context.resourceUpdates->updateDynamicBuffer(m_fragmentUbuf.get(), 0, fragmentUniforms.size() * sizeof(float), fragmentUniforms.data());
}

void CubeNode::render(RhiContext &context)
{
	context.cb->setGraphicsPipeline(m_pipeline.get());
	context.cb->setShaderResources();
	
	const QRhiCommandBuffer::VertexInput vbufBinding(m_vbuf.get(), 0);
	context.cb->setVertexInput(0, 1, &vbufBinding);
	
	context.cb->draw(36);
}

void CubeNode::releaseResources()
{
	qDebug() << "CubeNode::releaseResources()";
	m_texture.reset();
	m_sampler.reset();
	m_vbuf.reset();
	m_vertexUbuf.reset();
	m_fragmentUbuf.reset();
	m_srb.reset();
	m_pipeline.reset();
}

QString CubeNode::toString() const
{
	QString result;
	QTextStream stream(&result);
	
	stream << "CubeNode("
		   << "ModelMatrix: " << m_modelMatrix << ", "
		   << ")";
	
	return result;
}

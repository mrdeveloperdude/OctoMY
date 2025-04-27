#include "ShapeNode.hpp"

#include "construct/RhiContext.hpp"
#include "construct/Scene.hpp"
#include "construct/Utils.hpp"

#include <QDebug>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <array>
#include <cmath>
#include <vector>


static const auto TEXTURE_SIZE =  QSize(512, 512);

// Constructor: set default stride to 8 floats per vertex.
ShapeNode::ShapeNode(QSharedPointer<Scene> scene)
	: Layer(scene)
	, m_vertexStride(8 * sizeof(float))
{

}

ShapeNode::~ShapeNode() {
	// Resources will be released in releaseResources()
}

void ShapeNode::initialize(RhiContext &context)
{
	qDebug() << "ShapeNode::initialize()";
	makeSphere(3, 1.0f, context);
	if (m_vertexData.empty()) {
		qWarning() << "ShapeNode::initialize() - no vertex data set! Skipping resource creation.";
		return;
	}
	// Create vertex buffer with data size auto-determined.
	int dataSize = m_vertexData.size() * sizeof(float);
	m_vbuf.reset(context.m_rhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer, dataSize));
	m_vbuf->create();
	{
		auto resourceUpdates = context.m_rhi->nextResourceUpdateBatch();
		resourceUpdates->uploadStaticBuffer(m_vbuf.get(), 0, dataSize, m_vertexData.data());
		context.cb->resourceUpdate(resourceUpdates);
	}
	
	// Create uniform buffer for MVP matrix (64 bytes)
	m_ubuf.reset(context.m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, 128));
	m_ubuf->create();
	
	
	m_texture.reset(context.m_rhi->newTexture(QRhiTexture::RGBA8, TEXTURE_SIZE));
	m_texture->create();
	qDebug()<< "TEXTURE" << m_texture->pixelSize() << "x"<< m_texture->depth();
	
	m_sampler.reset(context.m_rhi->newSampler(QRhiSampler::Linear, QRhiSampler::Linear, QRhiSampler::None, QRhiSampler::Repeat, QRhiSampler::Repeat));
	m_sampler->create();
	
	// Setup shader resource bindings.
	m_srb.reset(context.m_rhi->newShaderResourceBindings());
	m_srb->setBindings({
		QRhiShaderResourceBinding::uniformBuffer(0, QRhiShaderResourceBinding::VertexStage, m_ubuf.get()),
		QRhiShaderResourceBinding::uniformBuffer(1, QRhiShaderResourceBinding::FragmentStage, m_ubuf.get()),
		QRhiShaderResourceBinding::sampledTexture(2, QRhiShaderResourceBinding::FragmentStage, m_texture.get(), m_sampler.get())
	});
	m_srb->create();
	
	
	
	// Setup pipeline.
	m_pipeline.reset(context.m_rhi->newGraphicsPipeline());
	m_pipeline->setDepthTest(true);
	m_pipeline->setDepthWrite(true);
	m_pipeline->setCullMode(QRhiGraphicsPipeline::Back);
	m_pipeline->setShaderStages({
		{ QRhiShaderStage::Vertex, getShader(QLatin1String(":/cube.vert.qsb")) },
		{ QRhiShaderStage::Fragment, getShader(QLatin1String(":/cube.frag.qsb")) }
	});
	QRhiVertexInputLayout inputLayout;
	inputLayout.setBindings({ { static_cast<quint32>(m_vertexStride) } }); // Cast to quint32
	inputLayout.setAttributes({
		{ 0, 0, QRhiVertexInputAttribute::Float3, 0 },                      // position at offset 0
		{ 0, 1, QRhiVertexInputAttribute::Float2, 3 * sizeof(float) },       // UV at offset 3 floats
		{ 0, 2, QRhiVertexInputAttribute::Float3, 5 * sizeof(float) }        // normal at offset 5 floats
	});
	m_pipeline->setSampleCount(context.m_sampleCount);
	m_pipeline->setVertexInputLayout(inputLayout);
	m_pipeline->setShaderResourceBindings(m_srb.get());
	m_pipeline->setRenderPassDescriptor(context.renderPassDescriptor);
	m_pipeline->create();

}
#include <array>

void ShapeNode::update(RhiContext &context, qint64 interval)
{
	Q_UNUSED(interval);
	// Update m_modelMatrix as needed; here we assume it's already set.
	// Compute the model matrix. (For a static shape, m_modelMatrix may be identity.)
	QMatrix4x4 modelMatrix = m_modelMatrix; // Or modify based on animation.
	
	// Retrieve camera matrices.
	QMatrix4x4 viewMatrix = mScene->camera()->viewMatrix();
	QMatrix4x4 projectionMatrix = mScene->camera()->projectionMatrix();
	
	// Compute the combined MVP matrix: projection * view * model.
	QMatrix4x4 mvp = projectionMatrix * viewMatrix * modelMatrix;
	
	// Prepare a uniform array for 32 floats (128 bytes):
	std::array<float, 32> uniformValues;
	
	// Copy the MVP matrix (first 16 floats).
	const float *mvpData = mvp.constData();
	for (int i = 0; i < 16; ++i)
		uniformValues[i] = mvpData[i];
	
	// Copy the model matrix (next 16 floats).
	const float *modelData = modelMatrix.constData();
	for (int i = 0; i < 16; ++i)
		uniformValues[16 + i] = modelData[i];
	
	context.ensureResourceUpdates();
	context.resourceUpdates->updateDynamicBuffer(m_ubuf.get(), 0, uniformValues.size() * sizeof(float), uniformValues.data());
}


void ShapeNode::render(RhiContext &context)
{
	if (!m_vbuf)
		return;
	context.cb->setGraphicsPipeline(m_pipeline.get());
	context.cb->setShaderResources();
	const QRhiCommandBuffer::VertexInput vbufBinding(m_vbuf.get(), 0);
	context.cb->setVertexInput(0, 1, &vbufBinding);
	int vertexCount = m_vertexData.size() / (m_vertexStride / sizeof(float));
	context.cb->draw(vertexCount);
}

void ShapeNode::releaseResources()
{
	qDebug() << "ShapeNode::releaseResources()";
	m_vbuf.reset();
	m_ubuf.reset();
	m_srb.reset();
	m_pipeline.reset();
}

QString ShapeNode::toString() const
{
	return QString("ShapeNode with %1 vertices")
	.arg(m_vertexData.size() / (m_vertexStride / sizeof(float)));
}

void ShapeNode::setVertexData(const std::vector<float> &data, int stride, RhiContext &context)
{
	m_vertexData = data;
	m_vertexStride = stride;
	int dataSize = m_vertexData.size() * sizeof(float);
	// Recreate vertex buffer.
	if (m_vbuf)
		m_vbuf.reset();
	m_vbuf.reset(context.m_rhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer, dataSize));
	m_vbuf->create();
	auto resourceUpdates = context.m_rhi->nextResourceUpdateBatch();
	resourceUpdates->uploadStaticBuffer(m_vbuf.get(), 0, dataSize, m_vertexData.data());
	context.cb->resourceUpdate(resourceUpdates);
}
#include "utils/GeometryGenerator.hpp"


void ShapeNode::makeSphere(int subdivisions, float radius, RhiContext &context)
{
	std::vector<float> sphereData = GeometryGenerator::makeSphere(subdivisions, radius);
	setVertexData(sphereData, 8 * sizeof(float), context);
}

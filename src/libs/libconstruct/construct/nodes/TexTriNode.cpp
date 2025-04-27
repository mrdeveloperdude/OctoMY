#include "TexTriNode.hpp"

#include <QDebug>
#include <QFile>
#include <QFont>
#include <QImage>
#include <QPainter>

#include "TriNode.hpp"
#include "construct/Scene.hpp"
#include "construct/RhiContext.hpp"
#include "construct/Utils.hpp"

static float vertexData[] = {
	// Position         // Texture Coordinates
	0.0f,   0.5f,  0.0f,   0.5f, 1.0f,
	-0.5f,  -0.5f,  0.0f,   0.0f, 0.0f,
	0.5f,  -0.5f,  0.0f,   1.0f, 0.0f,
};


static const auto TEXTURE_SIZE =  QSize(512, 512);

void TexTriNode::initialize(RhiContext &context)
{
	qDebug() << "TexTriNode::initialize(" << context << ")";
	m_pipeline.reset();
	m_vbuf.reset(context.m_rhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer, sizeof(vertexData)));
	m_vbuf->create();
	
	m_ubuf.reset(context.m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, 64));
	m_ubuf->create();
	
	// Create texture
	m_texture.reset(context.m_rhi->newTexture(QRhiTexture::RGBA8, TEXTURE_SIZE));
	m_texture->create();
	qDebug() << "TEXTURE" << m_texture->pixelSize() << "x" << m_texture->depth();
	
	m_sampler.reset(context.m_rhi->newSampler(QRhiSampler::Linear, QRhiSampler::Linear, QRhiSampler::None, QRhiSampler::Repeat, QRhiSampler::Repeat));
	m_sampler->create();
	
	m_srb.reset(context.m_rhi->newShaderResourceBindings());
	m_srb->setBindings({
		QRhiShaderResourceBinding::uniformBuffer(0, QRhiShaderResourceBinding::VertexStage, m_ubuf.get()),
		QRhiShaderResourceBinding::sampledTexture(1, QRhiShaderResourceBinding::FragmentStage, m_texture.get(), m_sampler.get())
	});
	m_srb->create();
	
	// Setup pipeline
	m_pipeline.reset(context.m_rhi->newGraphicsPipeline());
	m_pipeline->setCullMode(QRhiGraphicsPipeline::None);
	m_pipeline->setShaderStages({
		{ QRhiShaderStage::Vertex, getShader(QLatin1String(":/texture.vert.qsb")) },
		{ QRhiShaderStage::Fragment, getShader(QLatin1String(":/texture.frag.qsb")) }
	});
	
	QRhiVertexInputLayout inputLayout;
	inputLayout.setBindings({ { 5 * sizeof(float) } });
	inputLayout.setAttributes({
		{ 0, 0, QRhiVertexInputAttribute::Float3, 0 },
		{ 0, 1, QRhiVertexInputAttribute::Float2, 3 * sizeof(float) }
	});
	m_pipeline->setSampleCount(context.m_sampleCount);
	m_pipeline->setVertexInputLayout(inputLayout);
	m_pipeline->setShaderResourceBindings(m_srb.get());
	m_pipeline->setRenderPassDescriptor(context.renderTarget->renderPassDescriptor());
	m_pipeline->create();
	
	auto resourceUpdates = context.m_rhi->nextResourceUpdateBatch();
	resourceUpdates->uploadStaticBuffer(m_vbuf.get(), vertexData);
	resourceUpdates->uploadTexture(m_texture.get(), dummyTexture(TEXTURE_SIZE, "TEX TRI"));
	context.cb->resourceUpdate(resourceUpdates);

}

void TexTriNode::update(RhiContext &context, qint64 interval)
{
	//qDebug() << "TriNode::update(" << context << ")";
	m_angle = fmod(m_angle + (interval/1000.0), (M_PI * 2.0));
	const auto rotation = cos(m_angle) * 90;
	
	// Get camera matrices from Scene
	QMatrix4x4 viewMatrix = mScene->camera()->viewMatrix();
	QMatrix4x4 projectionMatrix = mScene->camera()->projectionMatrix();
	
	// Compute model matrix for rotation
	QMatrix4x4 modelMatrix;
	modelMatrix.rotate(rotation, 0, 1, 0);
	
	// Final MVP matrix
	QMatrix4x4 mvp = projectionMatrix * viewMatrix * modelMatrix * m_modelMatrix;
	
	context.ensureResourceUpdates();
	context.resourceUpdates->updateDynamicBuffer(m_ubuf.get(), 0, 64, mvp.constData());
}

void TexTriNode::render(RhiContext &context)
{
	//qDebug() << "TexTriNode::render(" << context << ")";
	context.cb->setGraphicsPipeline(m_pipeline.get());
	context.cb->setShaderResources();
	const QRhiCommandBuffer::VertexInput vbufBinding(m_vbuf.get(), 0);
	context.cb->setVertexInput(0, 1, &vbufBinding);
	context.cb->draw(3);
}

void TexTriNode::releaseResources()
{
	qDebug() << "TexTriNode::releaseResources()";
	m_texture.reset();
	m_sampler.reset();
	m_vbuf.reset();
	m_ubuf.reset();
	m_srb.reset();
	m_pipeline.reset();
}

QString TexTriNode::toString() const
{
	QString result;
	QTextStream stream(&result);
	stream << "TriNode()";
	return result;
}

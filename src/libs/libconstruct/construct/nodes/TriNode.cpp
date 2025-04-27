#include "TriNode.hpp"

#include <QDebug>
#include <QFile>

#include "construct/Scene.hpp"
#include "construct/RhiContext.hpp"
#include "construct/Utils.hpp"

static float vertexData[] = {
	0.0f,   0.5f,   1.0f, 0.0f, 0.0f,
	-0.5f,  -0.5f,   0.0f, 1.0f, 0.0f,
	0.5f,  -0.5f,   0.0f, 0.0f, 1.0f,
};

void TriNode::initialize(RhiContext &context)
{
	qDebug() << "TriNode::initialize(" << context << ")";
	m_pipeline.reset();
	if (!m_pipeline) {
		m_vbuf.reset(context.m_rhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer, sizeof(vertexData)));
		m_vbuf->create();
		
		m_ubuf.reset(context.m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, 64));
		m_ubuf->create();
		
		m_srb.reset(context.m_rhi->newShaderResourceBindings());
		m_srb->setBindings({
			QRhiShaderResourceBinding::uniformBuffer(0, QRhiShaderResourceBinding::VertexStage, m_ubuf.get()),
		});
		m_srb->create();
		
		m_pipeline.reset(context.m_rhi->newGraphicsPipeline());
		m_pipeline->setCullMode(QRhiGraphicsPipeline::None);
		m_pipeline->setShaderStages({
			{ QRhiShaderStage::Vertex, getShader(QLatin1String(":/tri_color.vert.qsb")) },
			{ QRhiShaderStage::Fragment, getShader(QLatin1String(":/tri_color.frag.qsb")) }
		});
		QRhiVertexInputLayout inputLayout;
		inputLayout.setBindings({
			{ 5 * sizeof(float) }
		});
		inputLayout.setAttributes({
			{ 0, 0, QRhiVertexInputAttribute::Float2, 0 },
			{ 0, 1, QRhiVertexInputAttribute::Float3, 2 * sizeof(float) }
		});
		m_pipeline->setVertexInputLayout(inputLayout);
		m_pipeline->setShaderResourceBindings(m_srb.get());
		m_pipeline->setRenderPassDescriptor(context.renderTarget->renderPassDescriptor());
		m_pipeline->create();
		
		QRhiResourceUpdateBatch *resourceUpdates = context.m_rhi->nextResourceUpdateBatch();
		resourceUpdates->uploadStaticBuffer(m_vbuf.get(), vertexData);
		context.cb->resourceUpdate(resourceUpdates);
	}
}


void TriNode::update(RhiContext &context, qint64 interval)
{
	//qDebug() << "TriNode::update(" << context << ")";
	m_angle = fmod(m_angle + (interval/1000.0), (M_PI * 2.0));
	const auto rotation = cos(m_angle) * 90;
	
	// Get camera matrices from Scene
	auto viewMatrix = mScene->camera()->viewMatrix();
	auto projectionMatrix = mScene->camera()->projectionMatrix();
	
	// Compute model matrix for rotation
	QMatrix4x4 modelMatrix;
	modelMatrix.rotate(rotation, 0, 1, 0);
	
	// Final MVP matrix
	auto mvp = projectionMatrix * viewMatrix * modelMatrix * m_modelMatrix;
	
	context.ensureResourceUpdates();
	context.resourceUpdates->updateDynamicBuffer(m_ubuf.get(), 0, 64, mvp.constData());
}


void TriNode::render(RhiContext &context)
{
	//qDebug() << "TriNode::render(" << context << ")";
	context.cb->setGraphicsPipeline(m_pipeline.get());
	const QSize outputSize = context.renderTarget->pixelSize();
	context.cb->setViewport(QRhiViewport(0, 0, outputSize.width(), outputSize.height()));
	context.cb->setShaderResources();
	const QRhiCommandBuffer::VertexInput vbufBinding(m_vbuf.get(), 0);
	context.cb->setVertexInput(0, 1, &vbufBinding);
	context.cb->draw(3);
	
}


void TriNode::releaseResources()
{
	qDebug() << "TriNode::releaseResources()";
}


QString TriNode::toString() const
{
	QString result;
	QTextStream stream(&result);
	
	stream << "TriNode("
		   << ")";
	
	return result;
}

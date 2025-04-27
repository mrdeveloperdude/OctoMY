#include "IsoSurfaceNode.hpp"

#include "construct/RhiContext.hpp"
#include "construct/Scene.hpp"
#include "construct/Utils.hpp" // For dummyTexture(), assumed similar to CubeNode

#include <QDebug>
#include <QFile>
#include <QMatrix4x4>
#include <QTextStream>
#include <array>

static const QSize TEXTURE_SIZE(512, 512);

// Full-screen quad vertices (2D positions) for two triangles covering the screen
static const float quadVertices[] = {
	-1.0f, -1.0f,
	1.0f, -1.0f,
	-1.0f,  1.0f,
	-1.0f,  1.0f,
	1.0f, -1.0f,
	1.0f,  1.0f
};

IsoSurfaceNode::IsoSurfaceNode(QSharedPointer<Scene> scene)
	: Layer(scene)
{
	
}

IsoSurfaceNode::~IsoSurfaceNode()
{
	releaseResources();
}

void IsoSurfaceNode::initialize(RhiContext &context)
{
	qDebug() << "IsoSurfaceNode::initialize()";
	
	// Create vertex buffer for full-screen quad
	m_vbuf.reset(context.m_rhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer, sizeof(quadVertices)));
	m_vbuf->create();
	
	auto resourceUpdates = context.m_rhi->nextResourceUpdateBatch();
	resourceUpdates->uploadStaticBuffer(m_vbuf.get(), quadVertices);
	
	// Create dynamic uniform buffer for fragment shader uniforms (8 floats)
	const int fragUbufSize = 8 * sizeof(float);
	m_fragUbuf.reset(context.m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, fragUbufSize));
	m_fragUbuf->create();
	
	// Create texture for density/noise (used for bicubic sampling)
	m_texture.reset(context.m_rhi->newTexture(QRhiTexture::RGBA8, TEXTURE_SIZE));
	m_texture->create();
	resourceUpdates->uploadTexture(m_texture.get(), dummyTexture(TEXTURE_SIZE, "ISO_SURFACE"));
	
	// Create sampler for the texture
	m_sampler.reset(context.m_rhi->newSampler(QRhiSampler::Linear, QRhiSampler::Linear,
											  QRhiSampler::None, QRhiSampler::Repeat, QRhiSampler::Repeat));
	m_sampler->create();
	
	// Setup shader resource bindings:
	//   Binding 0: Fragment uniform buffer
	//   Binding 1: Sampled texture (density map)
	m_srb.reset(context.m_rhi->newShaderResourceBindings());
	m_srb->setBindings({
		QRhiShaderResourceBinding::uniformBuffer(0, QRhiShaderResourceBinding::FragmentStage, m_fragUbuf.get()),
		QRhiShaderResourceBinding::sampledTexture(1, QRhiShaderResourceBinding::FragmentStage, m_texture.get(), m_sampler.get())
	});
	m_srb->create();
	
	// Setup graphics pipeline
	m_pipeline.reset(context.m_rhi->newGraphicsPipeline());
	m_pipeline->setSampleCount(context.m_sampleCount);
	
	// Vertex input layout: 2 floats per vertex (position)
	QRhiVertexInputLayout inputLayout;
	inputLayout.setBindings({ { 2 * sizeof(float) } });
	inputLayout.setAttributes({
		{ 0, 0, QRhiVertexInputAttribute::Float2, 0 }
	});
	m_pipeline->setVertexInputLayout(inputLayout);
	
	// Set shader stages – assume the shaders are compiled (QSB) and available as resources.
	m_pipeline->setShaderStages({
		{ QRhiShaderStage::Vertex, getShader(QLatin1String(":/iso_surface.vert.qsb")) },
		{ QRhiShaderStage::Fragment, getShader(QLatin1String(":/iso_surface.frag.qsb")) }
	});
	
	m_pipeline->setShaderResourceBindings(m_srb.get());
	m_pipeline->setRenderPassDescriptor(context.renderPassDescriptor);
	m_pipeline->create();
	
	context.cb->resourceUpdate(resourceUpdates);
}

void IsoSurfaceNode::update(RhiContext &context, qint64 interval)
{
	// Update time (in seconds)
	m_time = fmod(m_time + (interval / 1000.0f), 10000.0f);
	
	// If needed, update m_viewportSize here from context (if the render target changes)
	
	// Update fragment uniform buffer:
	// Layout: [ iTime, iResolution.x, iResolution.y, iMouse.x, iMouse.y, iChannelResolution.x, iChannelResolution.y, padding ]
	std::array<float, 8> fragUniforms;
	fragUniforms[0] = m_time;
	fragUniforms[1] = static_cast<float>(m_viewportSize.width());
	fragUniforms[2] = static_cast<float>(m_viewportSize.height());
	// iMouse (not implemented): set to zero
	fragUniforms[3] = 0.0f;
	fragUniforms[4] = 0.0f;
	// iChannelResolution from the texture's size
	fragUniforms[5] = static_cast<float>(m_texture->pixelSize().width());
	fragUniforms[6] = static_cast<float>(m_texture->pixelSize().height());
	fragUniforms[7] = 0.0f; // padding
	
	context.ensureResourceUpdates();
	context.resourceUpdates->updateDynamicBuffer(m_fragUbuf.get(), 0, fragUniforms.size() * sizeof(float), fragUniforms.data());
}

void IsoSurfaceNode::render(RhiContext &context)
{
	context.cb->setGraphicsPipeline(m_pipeline.get());
	context.cb->setShaderResources();
	
	const QRhiCommandBuffer::VertexInput vbufBinding(m_vbuf.get(), 0);
	context.cb->setVertexInput(0, 1, &vbufBinding);
	
	// Draw 6 vertices (2 triangles for the quad)
	context.cb->draw(6);
}

void IsoSurfaceNode::releaseResources()
{
	qDebug() << "IsoSurfaceNode::releaseResources()";
	m_texture.reset();
	m_sampler.reset();
	m_vbuf.reset();
	m_fragUbuf.reset();
	m_srb.reset();
	m_pipeline.reset();
}

QString IsoSurfaceNode::toString() const
{
	QString result;
	QTextStream stream(&result);
	stream << "IsoSurfaceNode("
		   << "Time: " << m_time << ", "
		   //<< "ViewportSize: " << m_viewportSize
		   << ")";
	return result;
}

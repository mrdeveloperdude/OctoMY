#include "GroundPlane.hpp"

#include <QDebug>
#include <QDebugStateSaver>
#include <QFile>
#include <QFont>
#include <QImage>
#include <QPainter>

#include "construct/RhiContext.hpp"
#include "construct/Scene.hpp"
#include "construct/Utils.hpp"

/*
static float vertexData[] = {
	// Position (x, y, z)    // Texture Coordinates (u, v)
	-1.0f,  0.0f, -1.0f,    0.0f, 1.0f,  // Bottom-left
	 1.0f,  0.0f, -1.0f,    1.0f, 1.0f,  // Bottom-right
	 1.0f,  0.0f,  1.0f,    1.0f, 0.0f,  // Top-right
	-1.0f,  0.0f,  1.0f,    0.0f, 0.0f   // Top-left
};
*/
static float vertexData[] = {
	// Position (NDC)   // Texture Coordinates (u, v)
	-1.0f, -1.0f,  0.0f,  0.0f, 0.0f,  // Bottom-left
	1.0f, -1.0f,  0.0f,  1.0f, 0.0f,  // Bottom-right
	1.0f,  1.0f,  0.0f,  1.0f, 1.0f,  // Top-right
	-1.0f,  1.0f,  0.0f,  0.0f, 1.0f   // Top-left
};


static uint16_t indexData[] = {
	0, 1, 2,  // First triangle
	2, 3, 0   // Second triangle
};


QDebug operator<<(QDebug dbg, const GroundPlaneUniforms &uniforms)
{
	QDebugStateSaver saver(dbg);
	dbg = dbg.nospace();
	dbg << "GroundPlaneUniforms(" << "\n";
	dbg << "\tmvp: " << uniforms.mvp << ",\n";
	dbg << "\tinv_view_matrix: " << uniforms.inv_view_matrix << ",\n";
	dbg << "\tcamera_position: " << uniforms.camera_position << ",\n";
	dbg << "\tcenter_offset: " << uniforms.center_offset << ",\n";
	dbg << "\tgrid_color: " << uniforms.grid_color << ",\n";
	dbg << "\tchecker_color_a: " << uniforms.checker_color_b << ",\n";
	dbg << "\tchecker_color_b: " << uniforms.checker_color_b << ",\n";
	dbg << "\tunit_size: " << uniforms.unit_size << ",\n";
	dbg << "\tmajor_line_thickness: " << uniforms.major_line_thickness << ",\n";
	dbg << "\tminor_line_thickness: " << uniforms.minor_line_thickness << ",\n";
	dbg << "\tminor_line_alpha: " << uniforms.minor_line_alpha << ",\n";
	dbg << "\tfade_start: " << uniforms.fade_start << ",\n";
	dbg << "\tfade_end: " << uniforms.fade_end << ",\n";
	dbg << "\tsubdivisions: " << uniforms.subdivisions << "\n";
	dbg << ")";
	return dbg.space();
}


void GroundPlane::initialize(RhiContext &context)
{
	qDebug() << "GroundPlane::initialize(" << context << ")";
	m_pipeline.reset();
	
	if (!m_pipeline) {
		// Create Vertex Buffer
		m_vbuf.reset(context.m_rhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer, sizeof(vertexData)));
		m_vbuf->create();
		
		// Create Index Buffer
		m_ibuf.reset(context.m_rhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::IndexBuffer, sizeof(indexData)));
		m_ibuf->create();
		
		// Create Uniform Buffer
		m_ubuf.reset(context.m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, sizeof(GroundPlaneUniforms)));
		m_ubuf->create();
		
		m_srb.reset(context.m_rhi->newShaderResourceBindings());
		m_srb->setBindings({
			QRhiShaderResourceBinding::uniformBuffer(0, QRhiShaderResourceBinding::VertexStage | QRhiShaderResourceBinding::FragmentStage, m_ubuf.get())
		});
		m_srb->create();
		
		m_pipeline.reset(context.m_rhi->newGraphicsPipeline());
		m_pipeline->setCullMode(QRhiGraphicsPipeline::None);
		m_pipeline->setDepthTest(true);
		m_pipeline->setDepthWrite(true);
		m_pipeline->setShaderStages({
			{ QRhiShaderStage::Vertex, getShader(QLatin1String(":/ground_plane.vert.qsb")) },
			{ QRhiShaderStage::Fragment, getShader(QLatin1String(":/ground_plane.frag.qsb")) }
		});
		
		QRhiVertexInputLayout inputLayout;
		inputLayout.setBindings({ { 5 * sizeof(float) } });
		inputLayout.setAttributes({
			{ 0, 0, QRhiVertexInputAttribute::Float3, 0 },  // Position (vec3)
			{ 0, 1, QRhiVertexInputAttribute::Float2, 3 * sizeof(float) } // UV (vec2)
		});
		m_pipeline->setVertexInputLayout(inputLayout);
		m_pipeline->setShaderResourceBindings(m_srb.get());
		m_pipeline->setRenderPassDescriptor(context.renderTarget->renderPassDescriptor());
		m_pipeline->create();
		
		// Upload vertex and index data
		QRhiResourceUpdateBatch *resourceUpdates = context.m_rhi->nextResourceUpdateBatch();
		resourceUpdates->uploadStaticBuffer(m_vbuf.get(), vertexData);
		resourceUpdates->uploadStaticBuffer(m_ibuf.get(), indexData);
		context.cb->resourceUpdate(resourceUpdates);
	}
}


static inline void matcpy(const QMatrix4x4 &mat, std::array<float, 16> &out){
	std::copy(mat.constData(), mat.constData() + 16, out.begin());
}

void GroundPlane::update(RhiContext &context, qint64 interval)
{
	Q_UNUSED(interval);
	//qDebug() << "GroundPlane::update(" << context << ")";
	auto camera = mScene->camera();
	auto viewMatrix = camera->viewMatrix();
	auto projectionMatrix = camera->projectionMatrix();
	auto mvp = projectionMatrix * viewMatrix * m_modelMatrix;
	auto inv_view_matrix = camera->viewMatrix().inverted();
	auto inv_projection = projectionMatrix.inverted();
	
	uniforms.camera_position = camera->position().toVector4D();
	matcpy(mvp, uniforms.mvp);
	matcpy(inv_view_matrix, uniforms.inv_view_matrix);
	matcpy(inv_projection, uniforms.inv_projection);
	
	context.ensureResourceUpdates();
	//qDebug()<<uniforms;
	context.resourceUpdates->updateDynamicBuffer(m_ubuf.get(), 0, sizeof(GroundPlaneUniforms), &uniforms);
}


void GroundPlane::render(RhiContext &context)
{
	//qDebug() << "GroundPlane::render(" << context << ")";
	context.cb->setGraphicsPipeline(m_pipeline.get());
	
	const QSize outputSize = context.renderTarget->pixelSize();
	context.cb->setViewport(QRhiViewport(0, 0, outputSize.width(), outputSize.height()));
	
	context.cb->setShaderResources();
	
	// Bind both vertex and index buffers using setVertexInput()
	const QRhiCommandBuffer::VertexInput vbufBinding(m_vbuf.get(), 0);
	context.cb->setVertexInput(0, 1, &vbufBinding, m_ibuf.get(), 0, QRhiCommandBuffer::IndexUInt16);
	
	// Draw the indexed quad (6 indices -> 2 triangles)
	context.cb->drawIndexed(6);
}


void GroundPlane::releaseResources()
{
	qDebug() << "TriNode::releaseResources()";
	m_texture.reset();
	m_sampler.reset();
	m_vbuf.reset();
	m_ibuf.reset();
	m_ubuf.reset();
	m_srb.reset();
	m_pipeline.reset();
}

QString GroundPlane::toString() const
{
	QString result;
	QTextStream stream(&result);
	stream << "TriNode()";
	return result;
}

#include "Gizmo.hpp"

#include "construct/RhiContext.hpp"
#include "construct/nodes/Layer.hpp" // SceneNode must define worldTransform() and overlayMesh()
#include "construct/Utils.hpp"  // Provides getShader(QLatin1String)
#include "OverlayStyleSheet.hpp"

#include <QByteArray>
#include <rhi/qrhi.h>
#include <QVector>
#include <algorithm>
#include <array>
#include <utility>

// Our uniform block structure using C++ containers.
struct GizmoUniform {
	std::array<float, 16> mvp;      // Model-View-Projection matrix.
	std::array<float, 4> tintColor; // Tint color (RGBA).
	float tintOpacity;              // Tint opacity.
	std::array<float, 3> pad;       // Padding if needed.
};

namespace Gizmo {

// Static pointer to our tint pipeline.
static QRhiGraphicsPipeline *gizmoTintPipeline = nullptr;

void renderNodeOverlay(const Layer *node,
	OverlayStates states,
	const OverlayStyleSheet &styleSheet,
	RhiContext &context)
{
	// Resolve the effective style from our style sheet.
	OverlayStyle style = styleSheet.resolveStyle(states);
	// If no tint, outline, or handles are requested, skip overlay.
	if (style.tintOpacity <= 0.0f && !style.showOutline && !style.showHandles)
		return;
	
	// Retrieve the node's world transform.
	QMatrix4x4 worldMat = node->worldTransform();
	// Combine with view-projection matrix from context.
	QMatrix4x4 mvp = context.m_viewProjMatrix * worldMat;
	
	// Retrieve the overlay mesh from the node.
	const MeshData &mesh = node->overlayMesh();
	
	// Lazy-initialize the gizmo tint pipeline.
	if (!gizmoTintPipeline) {
		gizmoTintPipeline = context.m_rhi->newGraphicsPipeline();
		
		// Load shader stages using getShader from Utils.hpp.
		gizmoTintPipeline->setShaderStages({
			{ QRhiShaderStage::Vertex, getShader(QLatin1String(":/gizmo/tint.vert.qsb")) },
			{ QRhiShaderStage::Fragment, getShader(QLatin1String(":/gizmoTint.frag.qsb")) }
		});
		
		// Set up a vertex input layout.
		QRhiVertexInputLayout inputLayout;
		inputLayout.setBindings({ { int(sizeof(float) * 3) } });
		inputLayout.setAttributes({ { 0, 0, QRhiVertexInputAttribute::Float3, 0 } });
		gizmoTintPipeline->setVertexInputLayout(inputLayout);
		
		// Set the render pass descriptor from the context.
		gizmoTintPipeline->setRenderPassDescriptor(context.renderPassDescriptor);
		
		// Set up blending for translucency.
		QRhiGraphicsPipeline::TargetBlend blend;
		blend.enable = true;
		blend.srcColor = QRhiGraphicsPipeline::SrcAlpha;
		blend.dstColor = QRhiGraphicsPipeline::OneMinusSrcAlpha;
		blend.srcAlpha = QRhiGraphicsPipeline::One;
		blend.dstAlpha = QRhiGraphicsPipeline::Zero;
		blend.colorWrite = QRhiGraphicsPipeline::ColorMask(QRhiGraphicsPipeline::R |
														   QRhiGraphicsPipeline::G |
														   QRhiGraphicsPipeline::B |
														   QRhiGraphicsPipeline::A);
		gizmoTintPipeline->setTargetBlends({ blend });
		
		gizmoTintPipeline->create();
	}
	
	// Bind our tint pipeline.
	context.cb->setGraphicsPipeline(gizmoTintPipeline);
	
	// Prepare uniform data using C++ containers.
	GizmoUniform uniformData;
	std::copy(mvp.constData(), mvp.constData() + 16, uniformData.mvp.begin());
	uniformData.tintColor[0] = style.tintColor.redF();
	uniformData.tintColor[1] = style.tintColor.greenF();
	uniformData.tintColor[2] = style.tintColor.blueF();
	uniformData.tintColor[3] = style.tintColor.alphaF();
	uniformData.tintOpacity = style.tintOpacity;
	
	// Update the uniform buffer via the resource update batch.
	// Assumes that context.gizmoUniformBuffer is a QRhiBuffer* allocated for this purpose.
	QRhiResourceUpdateBatch *u = context.ensureResourceUpdates();
	u->updateDynamicBuffer(context.gizmoUniformBuffer, 0, sizeof(GizmoUniform), &uniformData);
	context.cb->resourceUpdate(u);
	
	// Bind vertex and index buffers.
	// In Qt6, QRhiCommandBuffer::VertexInput is defined as a std::pair<QRhiBuffer*, unsigned int>.
	auto vertexInput = std::make_pair(const_cast<QRhiBuffer*>(mesh.vertexBuffer), 0u);
	// Cast away const for the index buffer.
	QRhiBuffer *ibuf = const_cast<QRhiBuffer*>(mesh.indexBuffer);
	// Set the vertex input. Assumes indices are of type unsigned short.
	context.cb->setVertexInput(0, 1, &vertexInput, ibuf, 0, QRhiCommandBuffer::IndexUInt16);
	
	// Issue the draw call.
	context.cb->drawIndexed(mesh.indexCount);
	
	// Additional effects (outlines, handles) would be rendered with other pipelines.
}

} // namespace Gizmo

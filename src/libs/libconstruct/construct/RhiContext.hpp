#ifndef RHICONTEXT_H
#define RHICONTEXT_H

#include <QSize>
#include <QMatrix4x4>

class QRhi;
class QRhiBuffer;
class QRhiCommandBuffer;
class QRhiDepthStencilClearValue;
class QRhiRenderPassDescriptor;
class QRhiRenderTarget;
class QRhiResourceUpdateBatch;

class RhiContext
{
public:
	// The Rendering Hardware Interface (RHI) Instance and all relevant state
	QRhi *m_rhi{nullptr};
	QRhiCommandBuffer *cb{nullptr};
	QRhiRenderPassDescriptor *renderPassDescriptor{nullptr};
	QRhiResourceUpdateBatch *resourceUpdates{nullptr};
	QRhiRenderTarget *renderTarget{nullptr};
	QRhiDepthStencilClearValue *depthStencilClearValue{nullptr};
	// The view matrix. Set on every viewport change. Combined with scene camera on rendering to produce final matrix
	QMatrix4x4 m_viewProjMatrix;
	// Antialiasing setting
	int m_sampleCount{-1};
	// Pixel size setting
	QSize m_pixelSize;
	// Flag to remember if we need to re-create the scene (on change in rhi instance for example)
	bool mDirty{true};
	
	QRhiBuffer *gizmoUniformBuffer{nullptr};
	

public:
	RhiContext();

public:
	// Calculate m_viewProjMatrix from viewport size in rhi instance
	void updateViewMatrix();
	// create resourceUpdates if it does not exist
	QRhiResourceUpdateBatch *ensureResourceUpdates();	
public:
	// Serialize to string (for debugging)
	QString toString() const;

};

QTextStream &operator<<(QTextStream &stream, const RhiContext &context);
QDebug operator<<(QDebug dbg, const RhiContext &context);

#endif // RHICONTEXT_H

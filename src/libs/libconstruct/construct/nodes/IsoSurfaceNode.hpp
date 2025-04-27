#ifndef ISOSURFACENODE_HPP
#define ISOSURFACENODE_HPP

#include "Layer.hpp"

#include <QString>
#include <QSize>
#include <QSharedPointer>

// Forward declarations
class RhiContext;
class Scene;
class QRhiBuffer;
class QRhiTexture;
class QRhiSampler;
class QRhiShaderResourceBindings;
class QRhiGraphicsPipeline;

class IsoSurfaceNode : public Layer
{
private:
	// Vertex buffer for full-screen quad
	QSharedPointer<QRhiBuffer> m_vbuf;
	// Dynamic uniform buffer for fragment shader uniforms
	QSharedPointer<QRhiBuffer> m_fragUbuf;
	// Texture used for density/bicubic sampling
	QSharedPointer<QRhiTexture> m_texture;
	// Sampler for the texture
	QSharedPointer<QRhiSampler> m_sampler;
	// Shader resource bindings
	QSharedPointer<QRhiShaderResourceBindings> m_srb;
	// Graphics pipeline
	QSharedPointer<QRhiGraphicsPipeline> m_pipeline;
	
	// Uniform state
	float m_time;
	QSize m_viewportSize;
	

public:
	IsoSurfaceNode(QSharedPointer<Scene> scene);
	~IsoSurfaceNode();
	// Layer interface
public:
	void initialize(RhiContext &context) override;
	void update(RhiContext &context, qint64 interval) override;
	void render(RhiContext &context) override;
	void releaseResources() override;
	QString toString() const override;
};

#endif // ISOSURFACENODE_HPP

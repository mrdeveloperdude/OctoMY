#ifndef TEXTRINODE_HPP
#define TEXTRINODE_HPP

#include "construct/nodes/Layer.hpp"
#include <rhi/qrhi.h>
#include <memory>

class RhiContext;

class TexTriNode : public Layer {
	std::unique_ptr<QRhiBuffer> m_vbuf;
	std::unique_ptr<QRhiBuffer> m_ubuf;
	std::unique_ptr<QRhiSampler> m_sampler;
	std::unique_ptr<QRhiTexture> m_texture;
	std::unique_ptr<QRhiShaderResourceBindings> m_srb;
	std::unique_ptr<QRhiGraphicsPipeline> m_pipeline;
	QMatrix4x4 m_modelMatrix;
	qreal m_angle{0};

public:
	explicit TexTriNode(QSharedPointer<Scene> scene): Layer(scene){}

public:
	void initialize(RhiContext &context) override;
	void update(RhiContext &context, qint64 interval) override;
	void render(RhiContext &context) override;
	void releaseResources() override;
	QString toString() const override;
};


#endif // TEXTRINODE_HPP



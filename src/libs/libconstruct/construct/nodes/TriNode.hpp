#ifndef TRINODE_HPP
#define TRINODE_HPP


#include <QDebug>

#include "Layer.hpp"

class TriNode : public Layer
{
private:
	std::unique_ptr<QRhiBuffer> m_vbuf;
	std::unique_ptr<QRhiBuffer> m_ubuf;
	std::unique_ptr<QRhiShaderResourceBindings> m_srb;
	std::unique_ptr<QRhiGraphicsPipeline> m_pipeline;
	//QMatrix4x4 m_viewProjection;
	float m_angle = 0.0f;

public:
	explicit TriNode(QSharedPointer<Scene> scene): Layer(scene){}

	// SceneNode interface
public:
	void initialize(RhiContext &context) override;
	void update(RhiContext &context, qint64 interval) override;
	void render(RhiContext &context) override;
	void releaseResources() override;
	QString toString() const override;

};





#endif // TRINODE_HPP

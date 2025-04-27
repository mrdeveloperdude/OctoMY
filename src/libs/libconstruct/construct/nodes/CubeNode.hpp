#ifndef CUBENODE_H
#define CUBENODE_H

#include <QDebug>

#include "Layer.hpp"

class CubeNode : public Layer
{
private:
	std::unique_ptr<QRhiBuffer> m_vbuf;
	//std::unique_ptr<QRhiBuffer> m_ubuf;
	
	// Add new members for the two uniform buffers:
	std::unique_ptr<QRhiBuffer> m_vertexUbuf;    // for vertex shader (128 bytes)
	std::unique_ptr<QRhiBuffer> m_fragmentUbuf;  // for fragment shader (112 bytes)
	
	
	std::unique_ptr<QRhiShaderResourceBindings> m_srb;
	std::unique_ptr<QRhiGraphicsPipeline> m_pipeline;
	std::unique_ptr<QRhiSampler> m_sampler;
	std::unique_ptr<QRhiTexture> m_texture;
	QMatrix4x4 m_modelMatrix;
	qreal m_angle{0};
private:
	void updateTexture(QRhiResourceUpdateBatch *resourceUpdates);

public:
	explicit CubeNode(QSharedPointer<Scene> scene): Layer(scene){}

	// SceneNode interface
public:
	void initialize(RhiContext &context) override;
	void update(RhiContext &context, qint64 interval) override;
	void render(RhiContext &context) override;
	void releaseResources() override;
	QString toString() const override;
};




#endif // CUBENODE_H

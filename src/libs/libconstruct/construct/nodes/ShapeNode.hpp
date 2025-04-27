#ifndef SHAPENODE_HPP
#define SHAPENODE_HPP

#include "Layer.hpp"
#include <vector>

class RhiContext;

class ShapeNode : public Layer {
public:
	// Interleaved vertex data format: position (3 floats), UV (2 floats), normal (3 floats)
	std::vector<float> m_vertexData;
	// Stride in bytes per vertex (should be 8*sizeof(float) for our format)
	int m_vertexStride{0};
	
	// Graphics resources
	QSharedPointer<QRhiBuffer> m_vbuf;
	QSharedPointer<QRhiBuffer> m_ubuf;
	QSharedPointer<QRhiShaderResourceBindings> m_srb;
	QSharedPointer<QRhiGraphicsPipeline> m_pipeline;
	std::unique_ptr<QRhiSampler> m_sampler;
	std::unique_ptr<QRhiTexture> m_texture;
	
	QMatrix4x4 m_modelMatrix;
	qreal m_angle{0};
	
public:
	ShapeNode(QSharedPointer<Scene> scene);
	virtual ~ShapeNode() override;
	
	// SceneNode interface overrides
	virtual void initialize(RhiContext &context) override;
	virtual void update(RhiContext &context, qint64 interval) override;
	virtual void render(RhiContext &context) override;
	virtual void releaseResources() override;
	virtual QString toString() const override;
	
	// Set the vertex data and automatically recreate the vertex buffer.
	void setVertexData(const std::vector<float> &data, int stride, RhiContext &context);
	
	// Generate sphere geometry by subdividing an icosahedron into a sphere.
	void makeSphere(int subdivisions, float radius, RhiContext &context);
};

#endif // SHAPENODE_HPP

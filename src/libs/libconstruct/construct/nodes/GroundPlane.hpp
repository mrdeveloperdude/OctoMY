#ifndef GROUNDPLANE_HPP
#define GROUNDPLANE_HPP

#include "construct/nodes/Layer.hpp"

#include <rhi/qrhi.h>
#include <memory>

class RhiContext;


struct __attribute__((packed)) GroundPlaneUniforms {
	std::array<float, 16> mvp;
	std::array<float, 16> inv_view_matrix;
	std::array<float, 16> inv_projection;
	QVector4D camera_position;
	QVector4D center_offset;
	QVector4D grid_color{0.8f, 0.7f, 0.3f, 0.5f};
	QVector4D checker_color_a{0.2f, 0.2f, 0.2f, 0.2f};
	QVector4D checker_color_b{0.3f, 0.3f, 0.3f, 0.2f};
	float unit_size{1.0f};
	float major_line_thickness{2.0f};
	float minor_line_thickness{1.0f};
	float minor_line_alpha{0.3f};
	float fade_start{0.0f};
	float fade_end{100.0f};
	float subdivisions{5};
	float pad{0.0f};
};

QDebug operator<<(QDebug dbg, const std::array<float, 16> &arr);
QDebug operator<<(QDebug dbg, const GroundPlaneUniforms &uniforms);

class GroundPlane : public Layer {
private:
	std::unique_ptr<QRhiBuffer> m_vbuf;
	std::unique_ptr<QRhiBuffer> m_ibuf;
	std::unique_ptr<QRhiBuffer> m_ubuf;
	std::unique_ptr<QRhiSampler> m_sampler;
	std::unique_ptr<QRhiTexture> m_texture;
	std::unique_ptr<QRhiShaderResourceBindings> m_srb;
	std::unique_ptr<QRhiGraphicsPipeline> m_pipeline;
	
	GroundPlaneUniforms uniforms;

public:
	explicit GroundPlane(QSharedPointer<Scene> scene): Layer(scene){}

public:
	void initialize(RhiContext &context) override;
	void update(RhiContext &context, qint64 interval) override;
	void render(RhiContext &context) override;
	void releaseResources() override;
	QString toString() const override;
};


#endif // GROUNDPLANE_HPP



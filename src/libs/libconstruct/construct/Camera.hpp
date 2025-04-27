#ifndef CAMERA_HPP
#define CAMERA_HPP


#include <QMatrix4x4>
#include <QVector3D>

class Camera {
	QVector3D m_position;
	QVector3D m_target;
	QVector3D m_up;
	QMatrix4x4 m_viewMatrix;
	QMatrix4x4 m_projectionMatrix;
	
	float m_fov = 45.0f;
	float m_aspectRatio = 1.0f;
	float m_nearPlane = 0.1f;
	float m_farPlane = 1000.0f;
	
	void updateViewMatrix();
	
public:
	Camera();
	
	void setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
	void setPosition(const QVector3D &position);
	void setTarget(const QVector3D &target);
	void setUpVector(const QVector3D &up);
	void setAspectRatio(float aspectRatio);
	
	QMatrix4x4 viewMatrix() const { return m_viewMatrix; }
	QMatrix4x4 projectionMatrix() const { return m_projectionMatrix; }
	QVector3D position() const { return m_position; }
	QVector3D target() const { return m_target; }
	QVector3D upVector() const { return m_up; }
	
	// Reset
	void reset();

	// Zoom in/out
	void dolly(float distance);
	// Move left/right/up/down
	void pan(float dx, float dy);
	// Rotate around target
	void orbit(float yaw, float pitch);
	// Rotate camera itself (FPS-style)
	void lookAround(float yaw, float pitch);
};


#endif // CAMERA_HPP


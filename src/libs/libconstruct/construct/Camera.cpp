#include "Camera.hpp"

#include <QtMath>


Camera::Camera()
{
	reset();
}

void Camera::reset(){
	m_position = QVector3D(0.0f, 2.0f, 5.0f);
	m_target = QVector3D(0.0f, 0.0f, 0.0f);
	m_up = QVector3D(0.0f, 1.0f, 0.0f);
	m_viewMatrix.setToIdentity();
	m_projectionMatrix.setToIdentity();
	m_fov = 45.0f;
	m_aspectRatio = 1.0f;
	m_nearPlane = 0.1f;
	m_farPlane = 1000.0f;
	updateViewMatrix();
	setPerspective(45.0f, 1.0f, 0.1f, 1000.0f);
}

void Camera::setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
{
	m_fov = fov;
	m_aspectRatio = aspectRatio;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
	m_projectionMatrix.setToIdentity();
	m_projectionMatrix.perspective(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::setPosition(const QVector3D &position)
{
	m_position = position;
	updateViewMatrix();
}

void Camera::setTarget(const QVector3D &target)
{
	m_target = target;
	updateViewMatrix();
}

void Camera::setUpVector(const QVector3D &up)
{
	m_up = up.normalized();
	updateViewMatrix();
}

void Camera::setAspectRatio(float aspectRatio)
{
	m_aspectRatio = aspectRatio;
	setPerspective(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::updateViewMatrix()
{
	m_viewMatrix.setToIdentity();
	m_viewMatrix.lookAt(m_position, m_target, m_up);
}

// 🚀 **Camera Movements**

void Camera::dolly(float distance)
{
	//qDebug() << "dolly(" << distance << ")";
	QVector3D forward = (m_target - m_position).normalized();
	m_position += forward * distance;
	m_target += forward * distance;
	updateViewMatrix();
}

void Camera::pan(float dx, float dy)
{
	//qDebug() << "pan(" << dx << ", " << dy <<")";
	QVector3D right = QVector3D::crossProduct(m_target - m_position, m_up).normalized();
	QVector3D up = m_up.normalized();
	
	m_position += right * dx;
	m_target += right * dx;
	
	m_position += up * dy;
	m_target += up * dy;
	
	updateViewMatrix();
}

void Camera::orbit(float yaw, float pitch)
{
	//qDebug() << "orbit(" << yaw << ", " << pitch <<")";
	QVector3D direction = m_position - m_target;
	QMatrix4x4 rotation;
	
	// Apply yaw (Y-axis rotation)
	rotation.setToIdentity();
	rotation.rotate(yaw, m_up);
	//direction = rotation * direction;
	direction = rotation.map(direction);
	
	// Apply pitch (X-axis rotation)
	QVector3D right = QVector3D::crossProduct(m_up, direction).normalized();
	rotation.setToIdentity();
	rotation.rotate(pitch, right);
	direction = rotation.map(direction);
	
	m_position = m_target + direction;
	updateViewMatrix();
}

void Camera::lookAround(float yaw, float pitch)
{
	//qDebug() << "lookAround(" << yaw << ", " << pitch <<")";
	QMatrix4x4 rotation;
	rotation.rotate(yaw, m_up);
	
	QVector3D right = QVector3D::crossProduct(m_target - m_position, m_up).normalized();
	rotation.rotate(pitch, right);
	
	m_target = m_position + rotation.map(m_target - m_position);
	updateViewMatrix();
}

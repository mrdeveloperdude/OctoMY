#include "MetaBlobsWidget.hpp"

#include <QPainter>
#include <QEvent>
#include <QResizeEvent>
#include <cmath>


MetaBlobsWidget::MetaBlobsWidget(QWidget *parent)
	: QWidget(parent)
	, animationTimer(new QTimer(this))
	, canvasFront(nullptr)
	, canvasBack(nullptr)
	, time(0.0f)
{
	// Timer to trigger repaint
	connect(animationTimer, &QTimer::timeout, this, [=]() {
		update();
	});
	animationTimer->start(1000 / 60);
	setupBlobs();
}

MetaBlobsWidget::~MetaBlobsWidget() {
	delete canvasFront;
	delete canvasBack;
}

void MetaBlobsWidget::setupBlobs() {
	// Initialize 3 blobs with different properties
	blobs.append({ QVector3D(-1.0f, 0.0f, 0.0f), 10.0f, 1.0f, 0.0f, 0.0f, 32.0f });
	blobs.append({ QVector3D(1.0f, 0.0f, 0.0f), 5.0f, 0.0f, 1.0f, 0.0f, 64.0f });
	blobs.append({ QVector3D(0.0f, 1.0f, 0.0f), 2.0f, 0.0f, 0.0f, 1.0f, 128.0f });
}

void MetaBlobsWidget::resizeEvent(QResizeEvent *event) {
	// Recreate canvases when window is resized (double-buffering)
	delete canvasFront;
	delete canvasBack;
	canvasFront = new QImage(event->size(), QImage::Format_RGB32);
	canvasBack = new QImage(event->size(), QImage::Format_RGB32);
	QWidget::resizeEvent(event);
}

void MetaBlobsWidget::paintEvent(QPaintEvent *) {
	if (!canvasFront || !canvasBack) return;
	updateBlobsPosition();
	renderMetablobs();
	std::swap(canvasFront, canvasBack);
	QPainter painter(this);
	painter.drawImage(0, 0, *canvasFront);
}

void MetaBlobsWidget::updateBlobsPosition() {
	// Update blob positions in a circular motion
	time += 0.01f;
	float radius = 2.0f;
	blobs[0].position = QVector3D(radius * cos(time), 0.0f, radius * sin(time));
	blobs[1].position = QVector3D(radius * cos(time + 2.0f), 0.0f, radius * sin(time + 2.0f));
	blobs[2].position = QVector3D(0.0f, radius * cos(time + 1.0f), radius * sin(time + 1.0f));
}



float MetaBlobsWidget::computeField(const QVector3D &point) {
	// Accumulate the field contribution from each blob
	float field = 0.0f;
	for (const Blob &blob : blobs) {
		float distance = (point - blob.position).length();
		if (distance > 0.0f) {
			field += blob.density / (distance * distance);
		}
	}
	return field;
}


void MetaBlobsWidget::renderMetablobs() {
	// Clear the canvas (back buffer)
	//memset(canvasBack->bits(), 0, canvasBack->byteCount());
	//memset(canvasBack->bits(), 0, canvasBack->height() * canvasBack->bytesPerLine());
	canvasBack->fill(0x880000);
	
	
	float width = canvasBack->width();
	float height = canvasBack->height();
	
	float epsilon = 0.01f;
	Q_UNUSED(epsilon);
	
	uchar *bits = canvasBack->bits();
	QVector3D rayOrigin(0.0f, 0.0f, -5.0f);  // Camera at z = -5
	Q_UNUSED(rayOrigin);
	for (float y = 0; y < height; ++y) {
		for (float x = 0; x < width; ++x) {
			float r=x/width;
			float g=0;
			float b=1;
			
			
			/*
			
			QVector3D rayDir = QVector3D((x - width / 2) / width, 
										 (y - height / 2) / height, 
										 1.0f).normalized();  // Ray direction through pixel
			
			float t = 0.0f;  // Distance along the ray
			bool hit = false;
			float maxDistance = 100.0f;  // Max marching distance
			
			// Ray marching loop
			while (t < maxDistance) {
				QVector3D point = rayOrigin + rayDir * t;  // Current point along the ray
				float distance = computeField(point);
				
				if (distance < epsilon) {
					// We've hit the surface of a blob
					hit = true;
					break;
				}
				
				// March further along the ray
				t += distance;  // Step forward by the distance to the surface
			}
			
			// If a hit occurred, perform shading
			if (hit) {
				QVector3D hitPoint = rayOrigin + rayDir * t;
				QVector3D normal = computeNormal(hitPoint);
				for (const Blob &blob : blobs) {
					QVector3D lightDir = QVector3D(1.0f, 1.0f, -1.0f).normalized();
					float diffuse = QVector3D::dotProduct(normal, lightDir);
					diffuse = qMax(0.0f, diffuse);  // Clamp to non-negative values
					
					// Accumulate color with shading
					r += blob.r * diffuse;
					g += blob.g * diffuse;
					b += blob.b * diffuse;
				}
			}
			*/
			
			
			
			
			r = qBound(0.0f, r, 1.0f);
			g = qBound(0.0f, g, 1.0f);
			b = qBound(0.0f, b, 1.0f);
			bits[0] = static_cast<uchar>(b * 255);
			bits[1] = static_cast<uchar>(g * 255);
			bits[2] = static_cast<uchar>(r * 255);
			bits[3] = 255;
			
			bits+=4;
			/*
			// Map the pixel to a 3D space (screen coordinates to world space)
			
			// Compute the field strength at this point
			
			// If the field exceeds the threshold, it's part of a blob
			if (field > 1.0f) {
				QVector3D normal = computeNormal(point);  // Compute normal for shading
				float r = 0.0f, g = 0.0f, b = 0.0f;
				
				// Accumulate color contributions from each blob
				for (const Blob &blob : blobs) {
					QVector3D lightDir = QVector3D(1.0f, 1.0f, -1.0f).normalized();
					float diffuse = QVector3D::dotProduct(normal, lightDir);
					diffuse = qMax(0.0f, diffuse);  // Clamp to non-negative values
					
					// Accumulate color with shading
					r += blob.r * diffuse;
					g += blob.g * diffuse;
					b += blob.b * diffuse;
				}
				// Set the pixel color
				setPixel(x, y, r, g, b+100);
			}
			setPixel(x, y, 0,1,0);
*/
		}
	}
}

QVector3D MetaBlobsWidget::computeNormal(const QVector3D &point) {
	// Use numerical differentiation to compute the normal at a point
	const float epsilon = 0.01f;
	QVector3D gradient;
	gradient.setX(computeField(point + QVector3D(epsilon, 0, 0)) - computeField(point));
	gradient.setY(computeField(point + QVector3D(0, epsilon, 0)) - computeField(point));
	gradient.setZ(computeField(point + QVector3D(0, 0, epsilon)) - computeField(point));
	
	// Normalize the gradient
	gradient.normalize();
	return gradient;
}

QVector3D MetaBlobsWidget::projectTo2D(const QVector3D &point) {
	// Simple orthogonal projection (ignoring depth for now)
	return QVector3D(point.x() * width(), point.y() * height(), point.z());
}

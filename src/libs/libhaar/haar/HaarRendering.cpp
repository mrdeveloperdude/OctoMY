#include "HaarRendering.hpp"

#include <QColor>
#include <QDebug>
#include <QPainter>

#include <limits>


void drawHaarCascade(QPainter &painter, const HaarCascade &cascade) {
	if (cascade.stages.isEmpty() || cascade.features.isEmpty()) {
		qWarning() << "Empty cascade or features in HaarCascade.";
		return;
	}
	
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing);
	
	const QSize windowSize = cascade.windowSize;
	if (windowSize.isEmpty()) {
		qWarning() << "Invalid window size in HaarCascade.";
		painter.restore();
		return;
	}
	
	// Scale painter to match the window size of the cascade
	painter.scale(1.0 / windowSize.width(), 1.0 / windowSize.height());
	painter.setPen(Qt::NoPen);
	
	// Iterate over stages
	for (const HaarStage &stage : cascade.stages) {
		// Iterate over weak classifiers
		for (const HaarWeakClassifier &weakClassifier : stage.weakClassifiers) {
			// Get the feature referenced by the weak classifier
			if (weakClassifier.featureIndex < 0 || weakClassifier.featureIndex >= cascade.features.size()) {
				qWarning() << "Invalid feature index in HaarWeakClassifier:" << weakClassifier.featureIndex;
				continue;
			}
			
			const HaarFeature &feature = cascade.features[weakClassifier.featureIndex];
			
			// Draw rectangles in the feature
			for (const HaarRect &rect : feature.rects) {
				// Determine the color based on the rectangle's weight
				int colorIntensity = static_cast<int>(qBound(0.0, rect.weight * 255, 255.0));
				QColor color = (rect.weight > 0) 
								   ? QColor(0, colorIntensity, 0, 128) // Positive weight: Green
								   : QColor(colorIntensity, 0, 0, 128); // Negative weight: Red
				
				painter.setBrush(QBrush(color));
				painter.drawRect(rect.region);
			}
		}
	}
	
	painter.restore();
}



// Calculate a bounding rectangle for the given cascade
QRect calculateBoundingRect(const HaarCascade &cascade) {
	qWarning() << "Calculating bounding rectangle for" << cascade.name;
	
	if (cascade.stages.isEmpty() || cascade.features.isEmpty()) {
		qWarning() << "Cascade has no stages or features.";
		return QRect();
	}
	const int BIG = std::numeric_limits<int>::max();
	int minX = +BIG;
	int minY = +BIG;
	int maxX = -BIG;
	int maxY = -BIG;
	
	// Iterate over each stage in the cascade
	for (const HaarStage &stage : cascade.stages) {
		// Iterate over weak classifiers in the stage
		for (const HaarWeakClassifier &weakClassifier : stage.weakClassifiers) {
			if (weakClassifier.featureIndex < 0 || weakClassifier.featureIndex >= cascade.features.size()) {
				qWarning() << "Invalid feature index in HaarWeakClassifier:" << weakClassifier.featureIndex;
				continue;
			}
			
			const HaarFeature &feature = cascade.features[weakClassifier.featureIndex];
			if (feature.rects.isEmpty()) {
				qWarning() << "Feature has no rectangles.";
				continue;
			}
			
			// Calculate bounding box for each rectangle in the feature
			for (const HaarRect &rect : feature.rects) {
				QRect region = rect.region;
				minX = std::min(minX, region.left());
				minY = std::min(minY, region.top());
				maxX = std::max(maxX, region.right());
				maxY = std::max(maxY, region.bottom());
			}
		}
	}
	
	// If no valid bounding box found, return empty QRect
	if (minX == +BIG || minY == +BIG || maxX == -BIG || maxY == -BIG) {
		qWarning() << "No valid bounding box found in the cascade.";
		return QRect();
	}
	
	QRect boundingRect(minX, minY, maxX - minX + 1, maxY - minY + 1); // Add 1 to include all pixels
	qWarning() << "Calculated bounding rectangle:" << boundingRect << "from" << minX << minY << maxX << maxY;
	return boundingRect;
}





static inline void renderError(QImage &image, const QString &message) {
	QPainter painter(&image);
	painter.fillRect(image.rect(), Qt::white);
	QFont font("Arial", 9);
	painter.setFont(font);
	painter.setPen(QPen(Qt::red, 2));
	QRect textRect = image.rect();
	painter.drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, message);
	painter.end();
}

/*
for(const auto &feature:cascade.features){
	for(const auto &rect:feature.rects){
		QRect scaledRect(
			offsetX + rect.region.left() * scale,
			offsetY + rect.region.top() * scale,
			rect.region.width() * scale,
			rect.region.height() * scale
			);
		painter.drawRect(scaledRect);
		painter.drawLine(scaledRect.center(), imageCenter);
		rect_ct++;
	}
}
*/

/*

// Draw a centered preview of the cascade into the image, at the largest possible size that maintains aspect ratio
void previewHaarCascade(QImage &image, const HaarCascade &cascade, qreal scaleFactor, const HaarStyle &style) {
	if (cascade.stages.isEmpty()) {
		renderError(image, "Error: Cascade is empty");
		return;
	}
	if (image.isNull()) {
		qWarning() << "No image provided for rendering";
		return;
	}
	auto boundingRect = calculateBoundingRect(cascade);
	if (boundingRect.isEmpty() || boundingRect.width() == 0 || boundingRect.height() == 0) {
		renderError(image, "Error: Invalid bounding box dimensions");
		return;
	}
	
	qreal scaleX = static_cast<qreal>(image.width()) / boundingRect.width();
	qreal scaleY = static_cast<qreal>(image.height()) / boundingRect.height();
	qreal scale = std::min(scaleX, scaleY) * scaleFactor;
	
	qreal offsetX = (image.width() - boundingRect.width() * scale) / 2.0 - boundingRect.left() * scale;
	qreal offsetY = (image.height() - boundingRect.height() * scale) / 2.0 - boundingRect.top() * scale;
	
	QPainter painter(&image);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(image.rect(), style.bgColor);
	painter.setPen(QPen(style.outlineColor, 1));
	const auto imageCenter=image.rect().center();
	int rect_ct{0};

	for (const HaarStage &stage : cascade.stages) {
		for (const HaarWeakClassifier &weakClassifier : stage.weakClassifiers) {
			if (weakClassifier.featureIndex < 0 || weakClassifier.featureIndex >= cascade.features.size()) {
				qWarning() << "Invalid feature index in HaarWeakClassifier:" << weakClassifier.featureIndex;
				continue;
			}
			const HaarFeature &feature = cascade.features[weakClassifier.featureIndex];
			for (const HaarRect &rect : feature.rects) {
				QRect scaledRect(
					offsetX + rect.region.left() * scale,
					offsetY + rect.region.top() * scale,
					rect.region.width() * scale,
					rect.region.height() * scale
					);
				painter.drawRect(scaledRect);
				painter.drawLine(scaledRect.center(), imageCenter);
				rect_ct++;
				const auto &fillColor = rect.weight > 0 ? style.leftColor : style.rightColor;
				painter.fillRect(scaledRect, fillColor);
			}
		}
	}
	
	qDebug()<<"rect_ct"<<rect_ct;
}
*/

QSet<QRect> rectSet;
QSet<int> featureIndexSet;

// Render a single rectangle
void renderHaarRect(QPainter &painter, const QPoint &imageCenter, const HaarRect &rect, qreal offsetX, qreal offsetY, qreal scale, const HaarStyle &style) {
	Q_UNUSED(imageCenter);
	QRect scaledRect(
		offsetX + rect.region.left() * scale,
		offsetY + rect.region.top() * scale,
		rect.region.width() * scale,
		rect.region.height() * scale
		);
	if(!rectSet.contains(scaledRect)){
		//qDebug()<<scaledRect;
		rectSet.insert(scaledRect);
	}
	const auto &fillColor = rect.weight > 0 ? style.leftColor : style.rightColor;
	painter.fillRect(scaledRect, fillColor);
	painter.drawRect(scaledRect);
	//painter.drawLine(scaledRect.center(), imageCenter);
}


// Render a single feature
void renderHaarFeature(QPainter &painter, const QPoint &imageCenter, const HaarFeature &feature, qreal offsetX, qreal offsetY, qreal scale, const HaarStyle &style, int &rect_ct) {
	for (const HaarRect &rect : feature.rects) {
		renderHaarRect(painter, imageCenter, rect, offsetX, offsetY, scale, style);
		rect_ct++;
	}
}


// Render a single stage
void renderHaarStage(QPainter &painter, const QPoint &imageCenter, const HaarStage &stage, const QVector<HaarFeature> &features, qreal offsetX, qreal offsetY, qreal scale, const HaarStyle &style, int &rect_ct) {
	for (const HaarWeakClassifier &weakClassifier : stage.weakClassifiers) {
		if (weakClassifier.featureIndex < 0 || weakClassifier.featureIndex >= features.size()) {
			qWarning() << "Invalid feature index in HaarWeakClassifier:" << weakClassifier.featureIndex;
			continue;
		}
		if(!featureIndexSet.contains(weakClassifier.featureIndex)){
			featureIndexSet.insert(weakClassifier.featureIndex);
			//qDebug()<<weakClassifier.featureIndex;
		}
		const HaarFeature &feature = features[weakClassifier.featureIndex];
		renderHaarFeature(painter, imageCenter, feature, offsetX, offsetY, scale, style, rect_ct);
	}
}


// Render the entire cascade into the image
void renderHaarCascade(QImage &image, const HaarCascade &cascade, qreal scaleFactor, const HaarStyle &style) {
	if (cascade.stages.isEmpty()) {
		renderError(image, "Error: Cascade is empty");
		return;
	}
	if (image.isNull()) {
		qWarning() << "No image provided for rendering";
		return;
	}
	auto boundingRect = calculateBoundingRect(cascade);
	if (boundingRect.isEmpty() || boundingRect.width() == 0 || boundingRect.height() == 0) {
		renderError(image, "Error: Invalid bounding box dimensions");
		return;
	}
	
	qreal scaleX = static_cast<qreal>(image.width()) / boundingRect.width();
	qreal scaleY = static_cast<qreal>(image.height()) / boundingRect.height();
	qreal scale = std::min(scaleX, scaleY) * scaleFactor;
	
	qreal offsetX = (image.width() - boundingRect.width() * scale) / 2.0 - boundingRect.left() * scale;
	qreal offsetY = (image.height() - boundingRect.height() * scale) / 2.0 - boundingRect.top() * scale;
	
	QPainter painter(&image);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(image.rect(), style.bgColor);
	painter.setPen(QPen(style.outlineColor, 1));
	const auto imageCenter = image.rect().center();
	
	int rect_ct = 0;
	for (const HaarStage &stage : cascade.stages) {
		renderHaarStage(painter, imageCenter, stage, cascade.features, offsetX, offsetY, scale, style, rect_ct);
	}
	// qDebug() << "rect_ct" << rect_ct;
}


void previewHaarCascade(QImage &image, const HaarCascade &cascade, qreal scaleFactor, const HaarStyle &style) {
	renderHaarCascade(image, cascade, scaleFactor, style);
}

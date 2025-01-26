#include "HaarFeatureExtractor.hpp"

#include <QDebug>
#include <qlabel.h>
#include <qpainter.h>

void HaarFeatureExtractor::ingest(const QImage &image) {
	originalImage = image;
	integralImage.set(image);
}



double HaarFeatureExtractor::evaluateFeature(const HaarFeature &feature, int scale, int offsetX, int offsetY) const {
	double featureSum{0.0};
	for (const HaarRect &rect : feature.rects) {
		QRect scaledRect(
			offsetX + rect.region.x() * scale,
			offsetY + rect.region.y() * scale,
			rect.region.width() * scale,
			rect.region.height() * scale
			);
		featureSum += rect.weight * integralImage.sumInRect(scaledRect);
	}
	return featureSum;
}


QVector<QRect> HaarFeatureExtractor::extract(const QSharedPointer<HaarCascade> &cascade, qint8 maxScale, qint8 stepSize) {
	QVector<QRect> detectedRegions;
	if (cascade.isNull()) {
		qWarning("No cascade provided");
		return detectedRegions;
	}
	if (integralImage.size().isEmpty()) {
		qWarning("Integral image not computed. Call set() first.");
		return detectedRegions;
	}
	
	int imageWidth = integralImage.width();
	int imageHeight = integralImage.height();
	int windowWidth = cascade->windowSize.width();
	int windowHeight = cascade->windowSize.height();
	qDebug()<<"integral image size: "<< integralImage.size();
	qDebug()<<"cascade window size: "<< cascade->windowSize;
	//QImage im_out(integralImage.size(), QImage::Format_RGBA8888);
	//QImage im_out("/home/leo/Desktop/face_test.png");
	QPainter painter(&originalImage);
	painter.setPen(Qt::green);
	for (int scale = 1; scale <= maxScale; ++scale) {
		qDebug()<<"scale: "<< scale;
		int scaledWidth = windowWidth * scale;
		int scaledHeight = windowHeight * scale;
		for (int y = 0; y <= imageHeight - scaledHeight; y += stepSize) {
			for (int x = 0; x <= imageWidth - scaledWidth; x += stepSize) {
				QRect candidateRegion(x, y, scaledWidth, scaledHeight);
				bool passedCascade = true;
				int tests_count{0};
				for (const HaarStage &stage : cascade->stages) {
					double stageSum{0.0};
					
					for (const HaarWeakClassifier &classifier : stage.weakClassifiers) {
						// Retrieve the feature by index
						const HaarFeature &feature = cascade->features[classifier.featureIndex];
						double featureValue = evaluateFeature(feature, scale, x, y);
						
						// Evaluate the weak classifier
						if (featureValue < classifier.threshold) {
							stageSum += classifier.leftVal;
						} else {
							stageSum += classifier.rightVal;
						}
					}
					tests_count++;
					if (stageSum < stage.stageThreshold) {
						passedCascade = false;
						break;
					}
				}
				if(tests_count>9){
					painter.setPen(passedCascade?Qt::green:Qt::red);
					painter.drawRect(candidateRegion);
					painter.setPen(Qt::white);
					painter.drawText(candidateRegion, QString("%1 %2").arg(scale).arg(tests_count));
					qDebug()<<"  test " << candidateRegion<< (passedCascade?"PASS":"MISS") << " after "<<tests_count;
				}
				if (passedCascade) {
					detectedRegions.append(candidateRegion);
				}
			}
		}
	}
	auto l_out = new QLabel();
	l_out->setWindowTitle("EXTRACTION IMAGE");
	l_out->setPixmap(QPixmap::fromImage(originalImage));
	l_out->show();
	return detectedRegions;
}


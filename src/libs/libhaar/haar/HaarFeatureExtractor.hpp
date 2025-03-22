#ifndef HAARFEATUREEXTRACTOR_HPP
#define HAARFEATUREEXTRACTOR_HPP

#include "HaarCascade.hpp"
#include "IntegralImage.hpp"

#include <QImage>
#include "uptime/SharedPointerWrapper.hpp"

class HaarFeatureExtractor {
private:
	IntegralImage<int> integralImage;
	QImage originalImage;

private:
	double evaluateFeature(const HaarFeature &feature, int scale, int offsetX, int offsetY) const;

public:
	HaarFeatureExtractor() = default;
	// Preprocess the input QImage to compute the integral image
	void ingest(const QImage &image);
	int evaluateRectangle(const QRect &rect);
	// Apply the Haar cascade on the preprocessed image
	QVector<QRect> extract(const QSharedPointer<HaarCascade> &cascade, qint8 maxScale = 4, qint8 stepSize = 8);

};

#endif // HAARFEATUREEXTRACTOR_HPP


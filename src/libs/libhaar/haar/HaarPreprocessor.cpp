#include "HaarPreprocessor.hpp"

#include <QImage>
#include <QVector>

// Function to preprocess the image
QImage preprocessImage(const QImage &input, bool grayscale, bool resize) {
	QImage wip;
	// Step 1: Convert to grayscale
	if(grayscale){
		wip = input.convertToFormat(QImage::Format_Grayscale8);
	}
	else{
		wip = input;
	}
	
	// Step 2: Normalize brightness and contrast (simple histogram equalization)
	QImage normalized = wip;
	int width = wip.width();
	int height = wip.height();
	QVector<int> histogram(256, 0);
	
	// Compute histogram
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			int intensity = qGray(wip.pixel(x, y));
			histogram[intensity]++;
		}
	}
	
	// Compute cumulative distribution
	QVector<int> cdf(256, 0);
	cdf[0] = histogram[0];
	for (int i = 1; i < 256; ++i) {
		cdf[i] = cdf[i - 1] + histogram[i];
	}
	
	// Normalize pixel values
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			int intensity = qGray(wip.pixel(x, y));
			int equalized = (cdf[intensity] - cdf[0]) * 255 / (width * height - cdf[0]);
			normalized.setPixel(x, y, qRgb(equalized, equalized, equalized));
		}
	}
	
	// Step 3: Downscale for efficiency (optional)
	if(resize){
		QImage resized = normalized.scaled(320, 240, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		return resized;
	}
	return normalized;
}

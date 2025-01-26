#ifndef HAARRENDERING_HPP
#define HAARRENDERING_HPP

#include "HaarCascade.hpp"


struct HaarStyle{
	const QColor leftColor;
	const QColor rightColor;
	const QColor outlineColor;
	const QColor bgColor;
	HaarStyle()
		: leftColor(0xFF, 0xFF, 0xFF, 0x15)
		, rightColor(0x00, 0x00, 0x00, 0x15)
		, outlineColor(0x00, 0x00, 0x00, 0xFF)
		, bgColor(0x7F, 0x00, 0x00, 0xFF)
	{
	}
};

QRect calculateBoundingRect(const HaarCascade &cascade);
void drawHaarCascade(QPainter &painter, const HaarCascade &cascade);
void previewHaarCascade(QImage &image, const HaarCascade &cascade, qreal scaleFactor = 0.9, const HaarStyle &style=HaarStyle());


#endif // HAARRENDERING_HPP

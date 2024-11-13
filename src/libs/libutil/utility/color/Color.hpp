#ifndef UTILITY_COLOR_HPP
#define UTILITY_COLOR_HPP

#include <QColor>


class QPainter;
class QRectF;
class QPointF;
class QImage;
class QString;


namespace utility
{

namespace color
{

inline QColor mix(const QColor& color1, const QColor& color2, qreal ratio=0.5, bool skipAlpha=false);


/**
 * @brief Mix two colors.
 * @param a One of the colors to be mixed.
 * @param b One of the colors to be mixed.
 * @param ratio The ratio between the colors. 0.3 will give a 30-70 mix.
 * @return The mixed color.
 */
QColor mix(const QColor& color1, const QColor& color2, qreal ratio, bool skipAlpha)
{
	ratio=qBound(0.0, ratio, 1.0);
	const qreal iratio=1.0-ratio;
	const qreal r = color1.redF() * iratio + color2.redF() * ratio;
	const qreal g = color1.greenF() * iratio + color2.greenF() * ratio;
	const qreal b = color1.blueF() * iratio + color2.blueF() * ratio;
	const qreal a = skipAlpha?1.0:(color1.alphaF() * iratio + color2.alphaF() * ratio);

	return QColor::fromRgbF(r, g, b, a);
}


inline float intensity(const QColor &color);
/**
 * @brief Return float intensity of color
 * @param color the input color to find intensity for
 * @return float intensity
 */
float intensity(const QColor &color) {
	float r = color.redF();
	float g = color.greenF();
	float b = color.blueF();
	float intensity = 0.2126f * r + 0.7152f * g + 0.0722f * b;
	return qBound(0.0f, intensity, 1.0f);
}


}
}


#endif
// UTILITY_COLOR_HPP

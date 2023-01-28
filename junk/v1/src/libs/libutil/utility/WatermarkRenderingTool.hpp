#ifndef WATERMARKRENDERINGTOOL_HPP
#define WATERMARKRENDERINGTOOL_HPP

#include <QColor>
#include <QSvgRenderer>

class QPainter;
class QSize;
class QString;
class QWidget;
class QDir;


class WatermarkRenderingTool
{
private:

	const QColor mTransparentColor;
	const QColor mFailColor;
	const QString mWatermarkPath;
	QSvgRenderer mSvgRendrer;
	QColor mSilhouetteForeground;
	QColor mSilhouetteBackground;

public:

	explicit WatermarkRenderingTool(QColor silhouetteForeground = Qt::transparent
									, QColor silhouetteBackground = Qt::transparent
									, QString watermarkPath = ":/images/octomy_watermark.svg");

	virtual ~WatermarkRenderingTool();

public:

	void renderWaterwmark(QPainter &painter, QSize originalWidgetSize);
};

#endif // WATERMARKRENDERINGTOOL_HPP

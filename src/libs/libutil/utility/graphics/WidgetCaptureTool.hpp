#ifndef WIDGETCAPTURETOOL_HPP
#define WIDGETCAPTURETOOL_HPP

#include "WatermarkRenderingTool.hpp"

#include <QColor>
#include <QSvgRenderer>

class QPainter;
class QSize;
class QString;
class QWidget;
class QDir;

class WidgetCaptureTool
{

private:
	WatermarkRenderingTool mWaterMarkRenderer;
	const QColor mInfoBarForeground;
	const QColor mInfoBarBackground;
	const QString mLogoPath;
	QSvgRenderer mSvgRendrer;

public:
	explicit WidgetCaptureTool(QColor infoBarForeground = QColor(0xFF, 0x00, 0xFF, 0xFF)
							   , QColor infoBarBackground = QColor(0x00, 0x00, 0xFF, 0xFF)
							   , QColor watermarkSilhouetteForeground = Qt::transparent
							   , QColor watermarkSilhouetteBackground = Qt::transparent
							   , QString watermarkPath = ":/images/octomy_watermark.svg"
							   , QString logoPath = ":/icons/octomy_logo.svg");

	virtual ~WidgetCaptureTool();

public:
	void captureWidget(QWidget *widget, QDir path, QString name, QString description="");

};

#endif
// WIDGETCAPTURETOOL_HPP

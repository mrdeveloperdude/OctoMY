#include "WatermarkRenderingTool.hpp"

#include "Standard.hpp"

#include <QPainter>
#include <QSize>
#include <QString>
#include <QWidget>
#include <QSvgRenderer>
#include <QDebug>
#include <QFileInfo>

WatermarkRenderingTool::WatermarkRenderingTool( QColor silhouetteForeground, QColor silhouetteBackground, QString watermarkPath)
	: mTransparentColor(Qt::transparent)
	, mFailColor(Qt::red)
	, mWatermarkPath(watermarkPath)
	, mSvgRendrer(watermarkPath)
	, mSilhouetteForeground(silhouetteForeground)
	, mSilhouetteBackground(silhouetteBackground)
{

}


WatermarkRenderingTool::~WatermarkRenderingTool()
{

}

void WatermarkRenderingTool::renderWaterwmark(QPainter &oPainter, QSize originalWidgetSize)
{
	OC_METHODGATE();
	QRect rect(QPoint(0,0), originalWidgetSize);

	//bool silhouetteEnabled(true);
	QPixmap doubleBuffer(originalWidgetSize);
	if(mSvgRendrer.isValid()) {
		doubleBuffer.fill(mTransparentColor);
		QPainter painter(&doubleBuffer);
		QSizeF originalSvgSize = QSizeF(mSvgRendrer.defaultSize());
		//qDebug()<<"originalWidgetSize="<<originalWidgetSize<<", originalSvgSize="<<originalSvgSize;
		originalSvgSize.scale(originalWidgetSize, Qt::KeepAspectRatio);
		//qDebug()<<"ajusted originalSvgSize="<<originalSvgSize;
		painter.save();
		painter.translate(QPointF((originalWidgetSize.width()-originalSvgSize.width())*0.5, (originalWidgetSize.height()-originalSvgSize.height())*0.5));
		painter.scale(originalSvgSize.width()/originalWidgetSize.width(), originalSvgSize.height()/originalWidgetSize.height());
		mSvgRendrer.render(&painter);
		painter.restore();
		if(mTransparentColor != mSilhouetteForeground) {
			painter.save();
			painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
			painter.fillRect(rect, mSilhouetteForeground);
			painter.restore();
		}
		/*
		oPainter.save();
		oPainter.setPen(mSilhouetteBackground);
		oPainter.drawRect(rect);
		oPainter.drawLine(rect.topLeft(),rect.bottomRight());
		oPainter.restore();
		*/
	} else {
		qWarning()<<"RENDERING watermark SVG FAIL";
		doubleBuffer.fill(mFailColor);
	}
	if(mTransparentColor != mSilhouetteBackground) {
		oPainter.fillRect(rect, mSilhouetteBackground);
	}
	oPainter.drawPixmap(rect, doubleBuffer, doubleBuffer.rect() );
}

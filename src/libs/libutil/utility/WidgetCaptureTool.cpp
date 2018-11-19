#include "WidgetCaptureTool.hpp"

#include "Utility.hpp"


#include "WatermarkRenderingTool.hpp"


#include <QPainter>
#include <QSize>
#include <QString>
#include <QWidget>
#include <QDir>

#include <QImageWriter>

#include <QTextDocument>


WidgetCaptureTool::WidgetCaptureTool(QColor infoBarForeground, QColor infoBarBackground, QColor silhouetteForeground, QColor silhouetteBackground, QString watermarkPath, QString logoPath)
	: mWaterMarkRenderer(silhouetteForeground, silhouetteBackground, watermarkPath)
	, mInfoBarForeground(infoBarForeground)
	, mInfoBarBackground(infoBarBackground)
	, mLogoPath(logoPath)
	, mSvgRendrer(logoPath)
{

}


WidgetCaptureTool::~WidgetCaptureTool()
{

}


void WidgetCaptureTool::captureWidget(QWidget *widget, QDir path, QString name, QString description)
{
	OC_FUNCTIONGATE();
	if(nullptr!=widget) {
		const bool infoBarEnabled=(QColor(Qt::transparent)!=mInfoBarBackground);
		description=description.trimmed();
		QTextDocument td;
		if(infoBarEnabled) {
			td.setHtml(QString("<h2>%1</h2>%2").arg(name).arg(description));
		}
		auto clearance=2;
		const QSize originalWidgetSize=widget->size();
		const QRect originalWidgetRect(QPoint(0,0), originalWidgetSize);

		td.setTextWidth((originalWidgetSize.width()*2)/3);
		const QSizeF infoBarSize(originalWidgetSize.width(), td.size().height()+clearance*2);
		const QRectF infoBarRect(QPointF(0, originalWidgetSize.height()), infoBarSize);



		const QSizeF logoAreaSize(infoBarSize.width()/3, qMin(infoBarSize.height(), 48.0));

		const QSize sizeWithInfoBar(originalWidgetSize.width(), static_cast<int>(originalWidgetSize.height()+infoBarSize.height()));
		const QRect rectWithInfoBar(QPoint(0,0), sizeWithInfoBar);
		QImage image(sizeWithInfoBar, QImage::Format_ARGB32_Premultiplied);
		image.fill(Qt::transparent);
		{
			QPainter painter(&image);
			{
				QPixmap px=widget->grab();
				painter.drawPixmap(originalWidgetRect, px, originalWidgetRect);
			}
			mWaterMarkRenderer.renderWaterwmark(painter, originalWidgetSize);

			if(infoBarEnabled) {
				// qDebug()<<"infoBarBackground= "<<mInfoBarBackground.name(QColor::HexArgb) <<", infoBarForeground= "<<mInfoBarForeground.name(QColor::HexArgb);
				painter.fillRect(infoBarRect, mInfoBarBackground);
				QSizeF logoSize = QSizeF(mSvgRendrer.defaultSize());
				//qDebug()<<"originalWidgetSize="<<originalWidgetSize<<", originalSvgSize="<<originalSvgSize;
				logoSize.scale(logoAreaSize, Qt::KeepAspectRatio);
				const QRectF logoAreaRect(QPointF(infoBarSize.width() - logoSize.width(), originalWidgetSize.height() + infoBarRect.height() - logoSize.height()), logoSize);
				//qDebug()<<"ajusted originalSvgSize="<<originalSvgSize;
				painter.save();
				//painter.translate(QPointF((infoBarSize.width()-logoSize.width()), originalWidgetSize.height() + (infoBarSize.height()-logoSize.height())*0.5));
				//painter.scale(logoSize.width()/sizeWithInfoBar.width(), logoSize.height()/sizeWithInfoBar.height());
				painter.translate(logoAreaRect.topLeft());
				painter.scale(logoSize.width()/sizeWithInfoBar.width(), logoSize.height()/sizeWithInfoBar.height());
				mSvgRendrer.render(&painter);
				painter.restore();
				painter.translate(clearance, originalWidgetSize.height() + clearance);
				td.drawContents(&painter);
				td.setDefaultStyleSheet(QString("*{color: %1;}").arg(mInfoBarForeground.name(QColor::HexArgb)));
			}
		}
		{
			const QString ext("png");
			const QString fileName(path.absoluteFilePath(name+"."+ext));
			QImageWriter writer;
			writer.setFileName(fileName);
			writer.setFormat(ext.toLocal8Bit());
			writer.setCompression(99999);
			//writer.setQuality(100); // Disabled because it somehow removes compression
			writer.setOptimizedWrite(true);
			writer.setText("Source", "http://octomy.org");
			const QString copyright=QString("Copyright 2016 - %1 OctoMY™ project, all rights reserved world wide.").arg(QString::number(qMax(2018, QDate::currentDate().year())));
			writer.setText("Copyright", copyright);
			writer.setText("Name", name);
			writer.setText("Path", fileName);
			if(!description.isEmpty()){
				writer.setText("Description", description);
			}
			writer.write(image);
		}
		//image.save(fileName);
	} else {
		qWarning()<<"ERROR: Widget was null during illustration, aborting...";
		return;
	}
}

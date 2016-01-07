#include "SvgWidget.hpp"
#include <QString>
#include <QPainter>
#include <QDebug>

SvgWidget::SvgWidget(QWidget *parent):
	QWidget(parent)
  , r(QString(":/icons/octomy_logo_bare.svg"))
{

}


void SvgWidget::paintEvent(QPaintEvent *){
	// Keep aspect ratio:
	QSize originalSvgSize = r.defaultSize();
	QSize originalWidgetSize = size();
	QPainter painter(this);
	originalSvgSize.scale(originalWidgetSize,Qt::KeepAspectRatio);
	painter.translate(QPointF(((qreal)originalWidgetSize.width()-(qreal)originalSvgSize.width())*0.5,((qreal)originalWidgetSize.height()-(qreal)originalSvgSize.height())*0.5));
	painter.scale((qreal)originalSvgSize.width()/(qreal)originalWidgetSize.width(), (qreal)originalSvgSize.height()/(qreal)originalWidgetSize.height());
	r.render(&painter);
}


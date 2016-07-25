#include "SvgWidget.hpp"
#include <QString>
#include <QPainter>
#include <QDebug>

SvgWidget::SvgWidget(QWidget *parent):
	QWidget(parent)
  , svg(0)
{

}

SvgWidget::~SvgWidget(){
}

void SvgWidget::paintEvent(QPaintEvent *){
	if(0!=svg){
		// Keep aspect ratio:
		QSize originalSvgSize = svg->defaultSize();
		QSize originalWidgetSize = size();
		QPainter painter(this);
		originalSvgSize.scale(originalWidgetSize,Qt::KeepAspectRatio);
		painter.translate(QPointF(((qreal)originalWidgetSize.width()-(qreal)originalSvgSize.width())*0.5,((qreal)originalWidgetSize.height()-(qreal)originalSvgSize.height())*0.5));
		painter.scale((qreal)originalSvgSize.width()/(qreal)originalWidgetSize.width(), (qreal)originalSvgSize.height()/(qreal)originalWidgetSize.height());
		//qDebug()<<"RENDERING SVG: "<<originalSvgSize;
		svg->render(&painter);
	}
}


//NOTE: this will override the svgURL propoerty by pointing directly to the given svg
void SvgWidget::setSvg(QSvgRenderer &svg){
	//qDebug()<<"Setting SVG by object";
	this->svg=&svg;
	lastURL="";
}


void SvgWidget::setSvgURL(QString url){
	//qDebug()<<"Setting SVG by URL:"<<url;
	svg=new QSvgRenderer(url);
	lastURL=url;
}
QString SvgWidget::svgURL() const{
	return lastURL;
}

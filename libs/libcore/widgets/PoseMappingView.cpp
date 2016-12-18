#include "PoseMappingView.hpp"

#include "../libutil/utility/Standard.hpp"

#include "PoseMappingWidget.hpp"
#include "puppet/PoseMapping.hpp"

#include <QPen>
#include <QPainter>
#include <QPushButton>
#include <QList>
#include <QRegularExpression>
#include <QDebug>


PoseMappingView::PoseMappingView(QWidget *parent)
	: QWidget(parent)
	, mWidget(nullptr)
{
	OC_METHODGATE();
}



void PoseMappingView::configure(PoseMappingWidget &widget)
{
	OC_METHODGATE();
	mWidget=&widget;
	update();
}

void PoseMappingView::paintEvent(QPaintEvent *)
{
	OC_METHODGATE();
	if(nullptr!=mWidget) {
		PoseMapping *mapping=mWidget->mapping();
		if(nullptr!=mapping) {
			// Get buttons so we can use their coordinates
			QList<QPushButton *> fromButtons = findChildren<QPushButton *>(QRegularExpression("pushButtonFrom.*"));
			QList<QPushButton *> toButtons = findChildren<QPushButton *>(QRegularExpression("pushButtonTo.*"));
			/*
						for(QPushButton *but:fromButtons) {
							qDebug()<<"FROM BUTTON: "<<but->geometry();
						}
						for(QPushButton *but:toButtons) {
							qDebug()<<"TO BUTTON: "<<but->geometry();
						}
						*/
			const qreal radius=6.0;
			QPainter painter(this);
			QPen pen;
			pen.setCapStyle(Qt::RoundCap);
			pen.setColor(Qt::red);
			pen.setWidthF(radius);
			painter.setBrush(Qt::NoBrush);
			painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

			const quint32 w=size().width();
			const quint32 h=size().height();


			const quint32 sz=mapping->size();

			const qreal step=(1.0)/((qreal)sz);
			qreal hue=step;
			QColor col;
			for(quint32 i=0; i<sz; ++i) {
				const quint32 m=mapping->map(i);
				const bool dif=(i!=m);


				col.setHslF(hue,0.5,0.25,dif?1.0:0.3);
				hue+=step;
				pen.setColor(col);
				painter.setPen(pen);

				//qDebug()<<"GOT MAP FROM "<<i<<" TO " <<m;
				QPushButton *fromButton=(i<fromButtons.size())?fromButtons.at(i):nullptr;
				QPushButton *toButton=(m<toButtons.size())?toButtons.at(m):nullptr;
				if((nullptr!=fromButton) &&(nullptr!=toButton) ) {
					QRect fromRect=fromButton->geometry();
					QRect toRect=toButton->geometry();
					QPointF fromPoint(fromRect.x()+fromRect.width()+radius*1.5,fromRect.y()+fromRect.height()/2);
					QPointF toPoint(toRect.x()-radius*1.5,toRect.y()+toRect.height()/2);
					QPointF midPoint=(fromPoint+toPoint)/2.0;
					//qDebug()<<"STEP "<<i<<" of "<<sz<<" has buttons: "<<fromButton<<toButton<<" and points "<<fromPoint<<midPoint<<toPoint;
					QPainterPath path;
					path.moveTo(fromPoint);
					path.cubicTo(QPointF(midPoint.x(),fromPoint.y()), QPointF(midPoint.x(),toPoint.y()), toPoint);
					painter.drawPath(path);
				}

			}
		}
	}
}

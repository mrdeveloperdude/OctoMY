#include "PoseMappingView.hpp"


#include "uptime/MethodGate.hpp"

#include "PoseMappingWidget.hpp"
#include "pose/PoseMapping.hpp"

#include <QPen>
#include <QPainter>
#include <QPushButton>
#include <QList>
#include <QRegularExpression>
#include <QPainterPath>
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
		auto mapping = mWidget->mapping();
		if(!mapping.isNull()) {
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
			painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
			//const quint32 w=static_cast<quint32>(size().width());
			//const quint32 h=static_cast<quint32>(size().height());
			const quint32 sz=mapping->size();

			const qreal step=(1.0)/(static_cast<qreal>(sz));
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
				QPushButton *fromButton=(i<static_cast<quint32>(fromButtons.size()))?fromButtons.at(static_cast<int>(i)):nullptr;
				QPushButton *toButton=(m<static_cast<quint32>(toButtons.size()))?toButtons.at(static_cast<int>(m)):nullptr;
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

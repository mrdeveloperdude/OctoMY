#include "PoseView.hpp"

#include "../libutil/utility/Standard.hpp"



#include <QPen>
#include <QPainter>
#include <QPushButton>
#include <QList>
#include <QRegularExpression>
#include <QDebug>

PoseView::PoseView(QWidget *parent)
	: QWidget(parent)
	, mPose(nullptr)
{

}

PoseView::~PoseView()
{

}

void PoseView::poseChanged(const Pose &pose)
{
	mPose=&pose;
	update();
}

void PoseView::paintEvent(QPaintEvent *)
{
	OC_METHODGATE();

	if(nullptr!=mPose) {
		const QRectF r=rect();
		const quint32 sz= mPose->size();
		const qreal step=r.width()/(qreal)sz;
		const qreal hueStep=1.0/(qreal)sz;
		const qreal margin=6.0;
		QPainter painter(this);
		painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

		qreal hue=hueStep;
		QColor col;
		qreal pos=0;
		for(quint32 i=0; i<sz; ++i) {
			const qreal m=mPose->value(i);
			col.setHslF(hue,0.5,0.25,1.0);
			painter.setBrush(col);
			painter.drawRect(QRectF(pos+margin,r.height()/2,pos-margin*2,(r.height()*0.5-margin)*m));
			hue+=hueStep;
			pos+=step;
		}
	}

}

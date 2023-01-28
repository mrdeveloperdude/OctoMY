#include "PoseView.hpp"

#include "uptime/MethodGate.hpp"

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
	OC_METHODGATE();
}

PoseView::~PoseView()
{
	OC_METHODGATE();
}

void PoseView::poseChanged(const Pose &pose)
{
	OC_METHODGATE();
	mPose=&pose;
	update();
}

void PoseView::paintEvent(QPaintEvent *)
{
	OC_METHODGATE();

	if(nullptr!=mPose) {
		const QRectF r=rect();
		const quint32 sz= static_cast<quint32>(mPose->size());
		const qreal step=r.width()/static_cast<qreal>(sz);
		const qreal hueStep=1.0/static_cast<qreal>(sz);
		const qreal margin=6.0;
		QPainter painter(this);
		painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

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

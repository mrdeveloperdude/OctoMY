#include "PingWidget.hpp"


#include "utility/time/HumanTime.hpp"
#include "uptime/MethodGate.hpp"

#include <QTime>
#include <QPainter>
#include <QRectF>



QRectF createCenteredRectF(qreal x, qreal y, qreal r)
{
	// The top-left corner is at (x - r, y - r) and the size is (2r x 2r)
	return QRectF(x - r, y - r, 2 * r, 2 * r);
}


bool Ping::drawPing(QPainter &painter, const qint64 &now, const qreal &speed, const int &w, const int &h) {
	const auto sz = qMax(w, h);
	const auto pingInterval = qAbs(now - time) * speed;
	const auto pingFactor{qBound(0.0, static_cast<qreal>(pingInterval)/static_cast<qreal>(sz), 2.0)};
	QRectF pingRect;
	// Wide or square aspect
	if(w >= h){
		pingRect = leftSide?createCenteredRectF(0, h/2, w*pingFactor) : createCenteredRectF(w, h/2, w*pingFactor); 
	}
	// Tall aspect
	else{
		pingRect = leftSide?createCenteredRectF(w/2, 0, h*pingFactor) : createCenteredRectF(w/2, h, h*pingFactor);
	}
	auto intensity{qBound(0.0, 1.0 - pingFactor, 1.0)};
	color.setAlphaF(intensity);
	painter.setPen(QPen(color, 2.0));
	color.setAlphaF(intensity * 0.5);
	painter.setBrush(color);
	painter.drawEllipse(pingRect);
	return (pingInterval < sz * 2);
}


PingWidget::PingWidget(QWidget *parent)
	: QWidget(parent)
	, mPings()
{
	OC_METHODGATE();
}


void PingWidget::paintEvent(QPaintEvent *)
{
	OC_METHODGATE();
	const auto now = utility::time::currentMsecsSinceEpoch<qint64>();
	const auto w = width();
	const auto h = height();
	QPainter painter(this);
	
	painter.fillRect(rect(), mBackgroundColor);
	bool doUpdate = false;
	
	// Iterate backwards through the list.
	for (int i = mPings.size() - 1; i >= 0; --i) {
		Ping &ping = mPings[i];
		bool stillActive = ping.drawPing(painter, now, mSpeed, w, h);
		if (!stillActive) {
			mPings.removeAt(i);
		} else {
			doUpdate = true;
		}
	}
	if (doUpdate) {
		update();
	}
}



void PingWidget::ping(QRgb rgba, bool leftSide){
	OC_METHODGATE();
	mLastPing = utility::time::currentMsecsSinceEpoch<qint64>();
	mPings.append(Ping(mLastPing, rgba, leftSide));
	update();
}



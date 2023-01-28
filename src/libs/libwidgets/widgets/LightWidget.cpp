#include "widgets/LightWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"
//#include "utility/time/HumanTime.hpp"

#include <QPainter>
#include <QtMath>
#include <QDebug>


const QColor LightWidget::sBgColor("black");
const QColor LightWidget::sHighlightColor(255,255,255,192);

const QColor LightWidget::sDefaultOKColor(0x7cc043);
const QColor LightWidget::sDefaultErrorColor(0xc07c43);


LightWidget::LightWidget(QWidget *parent, const QColor &color)
	: QWidget(parent)
	, mOn(false)
	, mBlinkTimer(this)
	//	, mBlinkStart(0)
{
	OC_METHODGATE();
	mBlinkTimer.setSingleShot(true);
	mBlinkTimer.setTimerType(Qt::PreciseTimer);
	if(!connect(&mBlinkTimer, &QTimer::timeout, this, [this]() {
				//auto actual=utility::time::currentMsecsSinceEpoch<quint64>()-mBlinkStart;
				//qDebug()<<"BLINK ENDED FOR " << this << " WITH "<<mBlinkTimer.interval()<< "ms TOOK "<<actual<<"ms";
				turnLightOff();
}, OC_CONTYPE_NON_UNIQUE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	setLightColor(color);
}


bool LightWidget::isLightOn() const
{
	OC_METHODGATE();
	return mOn;
}


void LightWidget::setLightOn(bool o)
{
	OC_METHODGATE();
	if (mOn == o) {
		return;
	}
	mOn = o;
	update();
}


void LightWidget::setLightColor(QColor c)
{
	OC_METHODGATE();
	if(mLightColor==c) {
		return;
	}
	mLightColor=c;
	mLightColorDarker=mLightColor.darker(300);
	QColor saturated=QColor::fromHslF(mLightColor.toHsv().hslHueF(), 1.0,0.5,1.0);
	const qreal r = qMin(1.0, saturated.redF());
	const qreal g = qMin(1.0, saturated.greenF());
	const qreal b = qMin(1.0, saturated.blueF());
	QGradientStops gs;
	qreal step=0.0;
	const qreal ofs=1.01;
	for(int i=0; i<11; ++i) {
		const qreal istep=1.0-step;
		const qreal p=qPow(1.2,step*10.0)-0.2;
		const qreal a=qMin(1.0, step*10.0);
		gs.append(QGradientStop(istep, QColor::fromRgbF(qBound(0.0, qPow(r+ofs, p)-ofs, 1.0), qBound(0.0, qPow(g+ofs, p)-ofs, 1.0), qBound(0.0, qPow(b+ofs, p)-ofs, 1.0), a)));
		step+=0.1;
	}
	mLightGradient.setStops(gs);
	update();
}


void LightWidget::toggleLight()
{
	mOn=!mOn;
	update();
}


void LightWidget::turnLightOff()
{
	OC_METHODGATE();
	setLightOn(false);
}


void LightWidget::turnLightOn()
{
	OC_METHODGATE();
	setLightOn(true);
}


void LightWidget::blink(int ms)
{
	OC_METHODGATE();
	setLightOn(true);
	//mBlinkStart=utility::time::currentMsecsSinceEpoch<quint64>();
	mBlinkTimer.start(ms);
	//qDebug()<<"BLINK STARTED FOR "<< this<< " WITH "<<mBlinkTimer.interval()<< "ms";
}


void LightWidget::paintEvent(QPaintEvent *)
{
	OC_METHODGATE();
	QPainter painter(this);
	painter.setPen(Qt::NoPen);
	painter.setRenderHint(QPainter::Antialiasing);
	const int border=2;
	const int border2=border*2;
	const int w=width()-1;
	const int h=height()-1;
	const int w2=w/2;
	const int h2=h/2;
	const int t=qMin(w,h)-border2*2;
	const int radius=t/2;
	const int diameter=radius*2;
	// Black bg
	painter.setBrush(sBgColor);
	painter.drawEllipse(w2-radius-border, h2-radius-border, diameter+border2, diameter+border2);
	// Light bg
	painter.setBrush(mLightColorDarker);
	painter.drawEllipse(w2-radius, h2-radius, diameter, diameter);
	// Light shine
	if(mOn) {
		mLightGradient.setCenter(w2, h2);
		mLightGradient.setFocalPoint(w2, h2);
		mLightGradient.setRadius(radius);
		painter.setBrush(mLightGradient);
		painter.drawEllipse(w2-radius-border, h2-radius-border, diameter+border2, diameter+border2);
	}
	// Highlight
	painter.setBrush(sHighlightColor);
	const int hp=radius/2;
	const int hr=radius/3;
	const int hd=hr*2;
	painter.drawEllipse(w2+hp-hr, h2-hp-hr, hd, hd);
}


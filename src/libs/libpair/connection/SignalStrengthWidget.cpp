#include "SignalStrengthWidget.hpp"



#include "uptime/MethodGate.hpp"

#include <QBitmap>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionButton>


SignalStrengthWidget::SignalStrengthWidget(
	QWidget *parent)
	: QWidget(parent)
{
	OC_METHODGATE();
}

SignalStrengthWidget::~SignalStrengthWidget()
{
	OC_METHODGATE();
}


QIcon SignalStrengthWidget::icon(){
	switch(mStrength){
		case(0):return QIcon(":/icons/comms/connection_low.svg");
		case(1):return QIcon(":/icons/comms/connection_medium.svg");
		case(2):return QIcon(":/icons/comms/connection_high.svg");
		default: return (mStrength>0)?QIcon(":/icons/comms/connection_full.svg"):QIcon(":/icons/comms/connection_off.svg");
	}
}


void SignalStrengthWidget::paintEvent(QPaintEvent *event)
{
	OC_METHODGATE();
	Q_UNUSED(event);
	QIcon ico = icon();
	if (ico.isNull()){
		return;
	}
	
	const QPalette &pal = palette();
	QColor iconColor;
	if (mStrength >= 0) {
		iconColor = pal.color(QPalette::Active,    QPalette::ButtonText);
	}
	else {
		iconColor = pal.color(QPalette::Active,    QPalette::Midlight);
	}
	QRect contentRect = rect();
	int side = qMin(contentRect.width(), contentRect.height());
	QSize iconSize(side, side);
	qreal dpr = devicePixelRatioF();
	QPixmap pix = ico.pixmap(iconSize * dpr, QIcon::On);
	
	
	
	
	pix.setDevicePixelRatio(dpr);
	QPixmap tinted(pix.size());
	tinted.fill(Qt::transparent);
	{
		QPainter tp(&tinted);
		tp.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
		tp.drawPixmap(0, 0, pix);
		tp.setCompositionMode(QPainter::CompositionMode_SourceIn);
		tp.fillRect(tinted.rect(), iconColor);
	}
	QSize finalSize = tinted.size() / dpr;
	int x = contentRect.x() + (contentRect.width()  - finalSize.width())  / 2;
	int y = contentRect.y() + (contentRect.height() - finalSize.height()) / 2;
	QPainter painter(this);
	painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
	painter.drawPixmap(x, y, tinted);
}


void SignalStrengthWidget::signalStrengthChanged(int strength){
	if(mStrength != strength){
		mStrength = strength;
		update();
	}
}

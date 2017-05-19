#include "widgets/LightWidget.hpp"

#include "utility/Standard.hpp"

#include <QPainter>


LightWidget::LightWidget(QWidget *parent,const QColor &color):
	QWidget(parent)
	, color(color)
	, on(false)
{
	OC_METHODGATE();
}

bool LightWidget::isLightOn() const
{
	OC_METHODGATE();
	return on;
}

void LightWidget::setLightOn(bool o)
{
	OC_METHODGATE();
	if (on == o) {
		return;
	}
	on = o;
	update();
}

void LightWidget::setLightColor(QColor c)
{
	OC_METHODGATE();
	color=c;
	update();
}

void LightWidget::toggleLight()
{
	on=!on;
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
/*

int LightWidget::heightForWidth(int w) const
{
	return w;
}
bool LightWidget::hasHeightForWidth() const
{
	return true;
}

*/

void LightWidget::paintEvent(QPaintEvent *)
{
	OC_METHODGATE();
	QPainter painter(this);
	painter.setPen(Qt::NoPen);
	painter.setRenderHint(QPainter::Antialiasing);
	QColor black("black");
	QColor white("white");
	painter.setBrush(black);
	int m=2,m2=m*2;
	int w=width()-1, h=height()-1;
	int w2=w/2;
	int h2=h/2;
	int dim=std::min(w,h)-m2-2;
	int a=dim/2,b=a*2;
	painter.drawEllipse(w2-a-m,h2-a-m,b+m2,b+m2);
	painter.setRenderHint(QPainter::Antialiasing);

	painter.setBrush(on?color:color.darker(300));
	painter.drawEllipse(w2-a,h2-a,b,b);
	white.setAlpha(192);
	painter.setBrush(white);
	int hl=b/4;
	painter.drawEllipse(w2+a/2-hl, h2-a/2-hl,hl*2,hl*2);
}


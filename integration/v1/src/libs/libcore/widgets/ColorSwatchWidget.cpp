
#include "ColorSwatchWidget.hpp"

#include "utility/Standard.hpp"

#include <QPainter>

#include <QColorDialog>


static QColor black("black");

void paintColorSwatch(QColor color, QPainter *painter, const QRect &rect, const QPalette &)
{
	OC_FUNCTIONGATE();
	painter->save();

	const int m=2, m2=m*2;
	const int w=rect.width()-1, h=rect.height()-1;
	const int w2=w/2;
	const int h2=h/2;
	const int dim=(qMin(w,h)*2)/3-m2-2;
	const int dim2=(qMin(w,h)*9)/10-m2-2;
	const int a=dim/2, b = a*2;
	const int a2=dim2/2, b2 = a2*2;



	//painter->drawRect(rect);
	painter->setPen(Qt::NoPen);
	painter->setRenderHint(QPainter::Antialiasing);


	painter->setBrush(black);

	painter->drawEllipse(w2-a2-m, h2-a2-m, b2 + m2, b2 + m2);

	painter->setBrush(color);
	painter->drawEllipse(w2-a-m, h2-a-m, b + m2, b + m2);
	painter->restore();
}


////////////////////////////////////////////////////////////////////////////////


ColorSwatchWidget::ColorSwatchWidget(QWidget *parent)
	: QWidget(parent)
	, mSwatchColor("red")
	, mPressedInside(false)
{
	OC_METHODGATE();
	setWindowTitle("Color Swatch");
}

ColorSwatchWidget::~ColorSwatchWidget()
{
	OC_METHODGATE();
}


void ColorSwatchWidget::paintEvent(QPaintEvent *)
{
	OC_METHODGATE();
	setUpdatesEnabled(false);
	QPainter painter(this);
	paint(&painter, rect(),palette());
	setUpdatesEnabled(true);
}


void ColorSwatchWidget::setSwatchColor(QColor c)
{
	OC_METHODGATE();
	mSwatchColor=c;
	update();
}


QColor ColorSwatchWidget::swatchColor()
{
	OC_METHODGATE();
	return mSwatchColor;
}




void ColorSwatchWidget::paint(QPainter *painter, const QRect &rect, const QPalette &palette) const
{
	OC_METHODGATE();
	paintColorSwatch(mSwatchColor, painter, rect, palette);
}



void ColorSwatchWidget::mousePressEvent(QMouseEvent *)
{
	OC_METHODGATE();
	mPressedInside=true;
}


void ColorSwatchWidget::mouseReleaseEvent(QMouseEvent *)
{
	OC_METHODGATE();
	if(mPressedInside) {
		QColor color = QColorDialog::getColor(mSwatchColor, nullptr, "Select color", QColorDialog::DontUseNativeDialog );
		if( color.isValid() ) {
			setSwatchColor(color);
		}
	}
	mPressedInside=false;
}



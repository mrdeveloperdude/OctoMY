#include "PenSwatchWidget.hpp"

#include "uptime/MethodGate.hpp"

#include "PenStyleDialog.hpp"

#include <QPainter>
#include <QColorDialog>


static QColor black("black");

void paintPenSwatch(QPen pen, QPainter *painter, const QRect &rect, const QPalette &)
{
	OC_FUNCTIONGATE();
	painter->save();

	const qreal w = rect.width() - 1, h = rect.height() - 1;
	const qreal r = qMin(w, h) / 2.0;
	const qreal x1 = r, y1 = r;
	const qreal x2 = w - r, y2 = r;

	painter->setRenderHint(QPainter::Antialiasing);

	painter->setPen(Qt::NoPen);
	painter->drawRect(rect);

	painter->setPen(pen);
	painter->drawLine(QLineF(x1, y1, x2, y2));

	painter->restore();
}


////////////////////////////////////////////////////////////////////////////////


PenSwatchWidget::PenSwatchWidget(QWidget *parent)
	: QWidget(parent)
	, mPressedInside(false)
{
	OC_METHODGATE();
	setWindowTitle("Pen Swatch");
}

PenSwatchWidget::~PenSwatchWidget()
{
	OC_METHODGATE();
}


void PenSwatchWidget::paintEvent(QPaintEvent *)
{
	OC_METHODGATE();
	setUpdatesEnabled(false);
	QPainter painter(this);
	paint(&painter, rect(),palette());
	setUpdatesEnabled(true);
}


void PenSwatchWidget::setSwatchPen(QPen pen)
{
	OC_METHODGATE();
	mSwatchPen=pen;
	update();
}


QPen PenSwatchWidget::swatchPen()
{
	OC_METHODGATE();
	return mSwatchPen;
}




void PenSwatchWidget::paint(QPainter *painter, const QRect &rect, const QPalette &palette) const
{
	OC_METHODGATE();
	paintPenSwatch(mSwatchPen, painter, rect, palette);
}



void PenSwatchWidget::mousePressEvent(QMouseEvent *)
{
	OC_METHODGATE();
	mPressedInside=true;
}


void PenSwatchWidget::mouseReleaseEvent(QMouseEvent *)
{
	OC_METHODGATE();
	if(mPressedInside) {
		QPen pen = PenStyleDialog::getPen(mSwatchPen, nullptr, "Select pen");
		setSwatchPen(pen);
	}
	mPressedInside=false;
}


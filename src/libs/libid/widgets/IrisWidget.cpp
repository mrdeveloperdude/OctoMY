#include "IrisWidget.hpp"



#include "security/PortableID.hpp"

#include "expression/IrisRendrer.hpp"

#include <QPainter>
#include <QFile>
#include <QRect>
#include <QSize>
#include <QDebug>

///////////////////////////////////////////////////////////////////////////

IrisWidget::IrisWidget(QWidget *parent)
	: QWidget(parent)
	, mDirty(true)
{
}

void IrisWidget::regenerateWidget()
{
	mDirty=true;
	update();
}

void IrisWidget::setPortableID(PortableID &id)
{
	identicon.setPortableID(id);
	regenerateWidget();
}


void IrisWidget::mouseDoubleClickEvent(QMouseEvent *)
{
	emit doubleClicked();
}




void IrisWidget::paintEvent(QPaintEvent *)
{
	OC_METHODGATE();
	const int threshold=10;
	const int w=width();
	const int h=height();
	const double aspect=(((double)w)/((double)h));
	const int sz=(aspect>1.0)?h:w;
	const int ox=floor((w-sz)/2);
	const int oy=floor((h-sz)/2);
	//check for resize change / dirty
	const int dbw=mDoubleBuffer.width();
	const int dbh=mDoubleBuffer.height();
	if (  mDirty || (qAbs(dbw-sz)>threshold) || (qAbs(dbh-sz)>threshold) ) {
		mDirty=false;
		mDoubleBuffer=QPixmap(sz,sz);
		mDoubleBuffer.fill(Qt::transparent);
		QPainter painter(&mDoubleBuffer);
		Personality p(identicon.id().id());
		IrisRendrer ir(p);
		QRect r(0,0,sz,sz);
		ir.draw(r, painter);
	}
	QPainter oPainter;
	oPainter.begin( this );
	oPainter.drawPixmap( QRect(ox, oy, sz, sz), mDoubleBuffer, mDoubleBuffer.rect() );
	oPainter.end();
}

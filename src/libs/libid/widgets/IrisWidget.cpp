#include "IrisWidget.hpp"


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
	mPid=id;
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
		IrisRendrer ir;
		ir.setPortableID(mPid);
		const int margin=(sz*100)/1000;
		QRect r(margin,margin,sz-margin*2,sz-margin*2);
		ir.draw(r, painter);
	}
	QPainter oPainter;
	oPainter.begin( this );
	oPainter.drawPixmap( QRect(ox, oy, sz, sz), mDoubleBuffer, mDoubleBuffer.rect() );
	oPainter.end();
}

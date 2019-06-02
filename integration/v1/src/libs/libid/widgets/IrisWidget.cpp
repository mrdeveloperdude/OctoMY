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
	, mIndex(0)
{
}

void IrisWidget::regenerateWidget()
{
	mDirty=true;
	update();
}

void IrisWidget::setIrixIndex(quint32 index)
{
	mIndex=index;
	regenerateWidget();
}

void IrisWidget::setPortableID(PortableID &id)
{
	mPid=id;
	identicon.setPortableID(id);
	regenerateWidget();
}

PortableID IrisWidget::portableID()
{
	return mPid;
}


void IrisWidget::mouseDoubleClickEvent(QMouseEvent *)
{
	emit doubleClicked();
}




void IrisWidget::paintEvent(QPaintEvent *)
{
	OC_METHODGATE();
	// If id is not set, we simply don't draw anything
	if("" == mPid.id()){
		return;
	}
	const int threshold=10;
	const int w=width();
	const int h=height();
	const double aspect=(((double)w)/((double)h));
	const int sz=(aspect>1.0)?h:w;
	const int margin=(sz*150)/1000;
	const int szm=sz-margin*2;
	const int ox=floor((w-szm)/2);
	const int oy=floor((h-szm)/2);
	//check for resize change / dirty
	const int dbw=mDoubleBuffer.width();
	const int dbh=mDoubleBuffer.height();
	if (  mDirty || (qAbs(dbw-szm)>threshold) || (qAbs(dbh-szm)>threshold) ) {
		mDirty=false;
		mDoubleBuffer=QPixmap(szm,szm);
		mDoubleBuffer.fill(Qt::transparent);
		QPainter painter(&mDoubleBuffer);
		IrisRendrer ir;
		ir.setPortableID(mPid);
		QRect r(0,0,szm,szm);
		ir.draw(r, painter, mIndex);
	}
	QPainter oPainter;
	oPainter.begin( this );
	oPainter.drawPixmap( QRect(ox, oy, szm, szm), mDoubleBuffer, mDoubleBuffer.rect() );
	oPainter.end();
}

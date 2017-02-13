#include "SvgWidget.hpp"
#include <QString>
#include <QPainter>
#include <QDebug>

#include "../libutil/utility/Standard.hpp"

SvgWidget::SvgWidget(QWidget *parent)
	: QWidget(parent)
	, mSVG(nullptr)
	, mSilhouetteEnabled(false)
	, mTransparentColor("#00000000")
	, mSilhouetteForeground("white")
	, mSilhouetteBackground(mTransparentColor)
	, mFailColor("red")
	, mDirty(true)
{

}

SvgWidget::~SvgWidget()
{
}


void SvgWidget::configure(bool doSilhouette, QColor solhouetteForeground, QColor silhouetteBackground)
{
	setSilhouetteEnabled(doSilhouette);
	setSilhouetteForeground(solhouetteForeground);
	setSilhouetteBackground(silhouetteBackground);
}

void SvgWidget::paintEvent(QPaintEvent *)
{
	OC_METHODGATE();
	const int threshold=0;
	const int w=width();
	const int h=height();
	const int dbw=mDoubleBuffer.width();
	const int dbh=mDoubleBuffer.height();
	if (  mDirty || (qAbs(dbw-w)>threshold) || (qAbs(dbh-h)>threshold) ) {
		//qDebug()<<"DIRTY!";
		mDirty=false;
		mDoubleBuffer=QPixmap(w,h);
		if(nullptr!=mSVG) {
			//qDebug()<<"Re-rendering double buffer";
			mDoubleBuffer.fill(mTransparentColor);
			QPainter painter(&mDoubleBuffer);
			// Keep aspect ratio:
			QSize originalSvgSize = mSVG->defaultSize();
			QSize originalWidgetSize = size();
			originalSvgSize.scale(originalWidgetSize,Qt::KeepAspectRatio);
			painter.save();
			painter.translate(QPointF(((qreal)originalWidgetSize.width()-(qreal)originalSvgSize.width())*0.5,((qreal)originalWidgetSize.height()-(qreal)originalSvgSize.height())*0.5));
			painter.scale((qreal)originalSvgSize.width()/(qreal)originalWidgetSize.width(), (qreal)originalSvgSize.height()/(qreal)originalWidgetSize.height());
			//qDebug()<<"RENDERING SVG: "<<originalSvgSize;
			mSVG->render(&painter);
			painter.restore();
			if(mSilhouetteEnabled) {
				painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
				painter.fillRect(QRect(0,0,w,h), mSilhouetteForeground);
			}
		} else {
			//qWarning()<<"FAIL";
			mDoubleBuffer.fill(mFailColor);
		}
	}
	QPainter oPainter;
	oPainter.begin( this );
	//qDebug()<<"blit dbuffer";
	if(mSilhouetteEnabled && (mTransparentColor != mSilhouetteBackground) ) {
		oPainter.fillRect(QRect(0,0,w,h), mSilhouetteBackground);
	}
	oPainter.drawPixmap( QRect(0,0,w,h), mDoubleBuffer, mDoubleBuffer.rect() );
	oPainter.end();
}


//NOTE: this will override the svgURL propoerty by pointing directly to the given svg
void SvgWidget::setSvg(QSvgRenderer &svg)
{
	//qDebug()<<"Setting SVG by object";
	this->mSVG=&svg;
	mLastURL="";
	mDirty=true;
}


void SvgWidget::setSvgURL(QString url)
{
	//qDebug()<<"Setting SVG by URL:"<<url;
	mSVG=new QSvgRenderer(url);
	mLastURL=url;
	mDirty=true;
}
QString SvgWidget::svgURL() const
{
	return mLastURL;
}


void SvgWidget::setSilhouetteEnabled(bool e)
{
	//qDebug()<<"Setting silhouette enabled: "<<e;
	if(mSilhouetteEnabled!=e) {
		mDirty=true;
		mSilhouetteEnabled=e;
	}
}

bool SvgWidget::silhouetteEnabled() const
{
	return mSilhouetteEnabled;
}


void SvgWidget::setSilhouetteForeground(QColor c)
{
	//qDebug()<<"Setting silhouette foreground: "<<c;
	if(mSilhouetteForeground!=c) {
		mDirty=true;
		mSilhouetteForeground=c;
	}
}

QColor SvgWidget::silhouetteForeground() const
{
	return mSilhouetteForeground;
}


void SvgWidget::setSilhouetteBackground(QColor c)
{
	//qDebug()<<"Setting silhouette background: "<<c;
	if(mSilhouetteBackground!=c) {
		mDirty=true;
		mSilhouetteBackground=c;
	}
}


QColor SvgWidget::silhouetteBackground() const
{
	return mSilhouetteBackground;
}















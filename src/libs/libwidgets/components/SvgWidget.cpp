#include "SvgWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


#include <QString>
#include <QPainter>
#include <QDebug>


SvgWidget::SvgWidget(QWidget *parent)
	: QWidget(parent)
	, mSVG(nullptr)
	, mSilhouetteEnabled(false)
	, mTransparentColor("#00000000")
	, mSilhouetteForeground("white")
	, mSilhouetteBackground(mTransparentColor)
	, mFailColor("red")
	, mDirty(true)
	, mConfigureHelperSvg("SvgWidget", true, false, false, true, false)
{
	OC_METHODGATE();
}

SvgWidget::~SvgWidget()
{
	OC_METHODGATE();
}


void SvgWidget::configure(bool doSilhouette, QColor solhouetteForeground, QColor silhouetteBackground)
{
	OC_METHODGATE();
	if(mConfigureHelperSvg.configure()) {
		setSilhouetteEnabled(doSilhouette);
		setSilhouetteForeground(solhouetteForeground);
		setSilhouetteBackground(silhouetteBackground);
	}
}

void SvgWidget::paintEvent(QPaintEvent *)
{
	OC_METHODGATE();
	if(mConfigureHelperSvg.isConfiguredAsExpected()) {
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
				painter.translate(QPointF(
									  (static_cast<qreal>(originalWidgetSize.width())-static_cast<qreal>(originalSvgSize.width()))*0.5
									  ,(static_cast<qreal>(originalWidgetSize.height())-static_cast<qreal>(originalSvgSize.height()))*0.5)
								 );
				painter.scale(
					static_cast<qreal>(originalSvgSize.width())/static_cast<qreal>(originalWidgetSize.width())
					,static_cast<qreal>(originalSvgSize.height())/static_cast<qreal>(originalWidgetSize.height())
				);
				//qDebug()<<"RENDERING SVG: "<<originalSvgSize;
				mSVG->render(&painter);
				painter.restore();
				if(mSilhouetteEnabled) {
					painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
					painter.fillRect(QRect(0, 0, w, h), mSilhouetteForeground);
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
	} else {
		QPainter oPainter;
		oPainter.begin( this );
		oPainter.fillRect(rect(), mFailColor);
		oPainter.end();
	}
}


//NOTE: this will override the svgURL property by pointing directly to the given svg
void SvgWidget::setSvg(QSvgRenderer &svg)
{
	OC_METHODGATE();
	if(true /*mConfigureHelperSvg.isConfiguredAsExpected() */) {
		//qDebug()<<"Setting SVG by object";
		this->mSVG=&svg;
		mLastURL="";
		mDirty=true;
	}
}


void SvgWidget::setSvgURL(QString url)
{
	OC_METHODGATE();
	if(true /* mConfigureHelperSvg.isConfiguredAsExpected()*/) {
		//qDebug()<<"Setting SVG by URL:"<<url;
		mSVG=OC_NEW QSvgRenderer(url);
		mLastURL=url;
		mDirty=true;
	}
}

QString SvgWidget::svgURL() const
{
	OC_METHODGATE();
	if(true /*mConfigureHelperSvg.isConfiguredAsExpected()*/) {
		return mLastURL;
	}
	return QString();
}


void SvgWidget::setSilhouetteEnabled(bool e)
{
	OC_METHODGATE();
	if(mConfigureHelperSvg.isConfiguredAsExpected()) {
		//qDebug()<<"Setting silhouette enabled: "<<e;
		if(mSilhouetteEnabled!=e) {
			mDirty=true;
			mSilhouetteEnabled=e;
		}
	}

}

bool SvgWidget::silhouetteEnabled() const
{
	OC_METHODGATE();
	if(mConfigureHelperSvg.isConfiguredAsExpected()) {
		return mSilhouetteEnabled;
	}
	return false;
}


void SvgWidget::setSilhouetteForeground(QColor c)
{
	OC_METHODGATE();
	if(mConfigureHelperSvg.isConfiguredAsExpected()) {
		//qDebug()<<"Setting silhouette foreground: "<<c;
		if(mSilhouetteForeground!=c) {
			mDirty=true;
			mSilhouetteForeground=c;
		}
	}

}

QColor SvgWidget::silhouetteForeground() const
{
	OC_METHODGATE();
	if(mConfigureHelperSvg.isConfiguredAsExpected()) {
		return mSilhouetteForeground;
	}
	return QColor();
}


void SvgWidget::setSilhouetteBackground(QColor c)
{
	OC_METHODGATE();
	if(mConfigureHelperSvg.isConfiguredAsExpected()) {
		//qDebug()<<"Setting silhouette background: "<<c;
		if(mSilhouetteBackground!=c) {
			mDirty=true;
			mSilhouetteBackground=c;
		}
	}
}


QColor SvgWidget::silhouetteBackground() const
{
	OC_METHODGATE();
	if(mConfigureHelperSvg.isConfiguredAsExpected()) {
		return mSilhouetteBackground;
	}
	return QColor();
}















#include "TrustSymbolWidget.hpp"

#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "utility/random/Random.hpp"

#include <QPainter>
#include <QFile>
#include <QDomDocument>
#include <QDebug>
#include <QSvgRenderer>


const QRgb TrustSymbolWidget::palette[]= {
	0x000000
	,0xffffff
	,0x883932
	,0x67b6bd
	,0x8b5429
	,0x574200
	,0xb86962
	,0x505050
	,0x8b3f96
	,0x55a049
	,0x40318d
	,0xbfce72
	,0x787878
	,0x94e089
	,0x7869c4
	,0x9f9f9f
};

const int TrustSymbolWidget::paletteSz(static_cast<int>(sizeof(palette) / sizeof(QRgb)));


TrustSymbolWidget::TrustSymbolWidget(QWidget *parent)
	: SvgWidget(parent)
	, mLevel(IGNORE)
	, mConfigureHelper("TrustSymbolWidget", true, false, false, true, false)
{
	OC_METHODGATE();
	Q_INIT_RESOURCE(icons);
	setWindowTitle("Trust Symbol");
}


void TrustSymbolWidget::configure()
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		SvgWidget::configure(true);

		mPulsatingTrustTimer.setTimerType(Qt::PreciseTimer);

		// Somewhat conservative FPS
		mPulsatingTrustTimer.setInterval(1000/30);
		mPulsatingTrustTimer.setSingleShot(false);
		if(!QObject::connect(&mPulsatingTrustTimer, SIGNAL(timeout()), this, SLOT(onPulsatingTrustTimer()), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect ";
		}
	}
}


void TrustSymbolWidget::setTrustLevel(TrustLevel level)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		mLevel = level;
		QString file;
		switch(level) {
		case(TRUST): {
			file=":/icons/trust.svg";
		}
		break;
		case(BLOCK): {
			file=":/icons/block.svg";
		}
		break;
		// default:
		case(REMOVE):
		case(IGNORE): {
			file=":/icons/ignore.svg";
		}
		break;
	
		}
		setSvgURL(file);
		updatePulsating();
	}
}


void TrustSymbolWidget::updatePulsating()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		const bool pulsating=QWidget::isVisible() && (TRUST==mLevel);
		//qDebug()<<"PULSATING="<<pulsating;
		setSilhouetteEnabled(pulsating);
		if(pulsating) {
			mPulsatingTrustTimer.start();
		} else {
			mPulsatingTrustTimer.stop();
		}
		update();
	}
}


void TrustSymbolWidget::mouseDoubleClickEvent(QMouseEvent *)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		emit doubleClicked();
	}
}


void TrustSymbolWidget::showEvent(QShowEvent *)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		updatePulsating();
	}
}


void TrustSymbolWidget::hideEvent(QHideEvent *)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		updatePulsating();
	}
}


void TrustSymbolWidget::onPulsatingTrustTimer()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		const auto lastColor=silhouetteForeground();
		QColor color=lastColor;
		size_t index=0;
		while(lastColor == color) {
			index=static_cast<size_t>(utility::random::qrand()%paletteSz);
			color=QColor(palette[index]);
		}
		// qDebug()<<"INDEX: "<<index<<", COLOR: "<<color;
		setSilhouetteForeground(color);
		setSilhouetteEnabled(true);
		update();
	}
}

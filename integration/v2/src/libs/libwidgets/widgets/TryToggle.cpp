#include "TryToggle.hpp"
#include "ui_TryToggle.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


TryToggle::TryToggle(QWidget *parent) :
	QWidget(parent)
	, ui(OC_NEW Ui::TryToggle)
	, mState(OFF)
	, mOffText("OFF")
	, mGoingOnText("GOING ON")
	, mOnText("ON")
	, mGoingOffText("GOING OFF")

	, mTimerToggle(false)
{
	OC_METHODGATE();
	ui->setupUi(this);
	mTimer.setInterval(1000/5); //Multiple of 60Hz gives stutter free animation
	mTimer.setTimerType(Qt::PreciseTimer);
	connect(&mTimer,SIGNAL(timeout()), this, SLOT(onTimeout()));
	qRegisterMetaType<TryToggleState>("TryToggleState");
	// Leave off-palette unchanged
	mOffPalette=this->palette();
	// Set default on palette to white text on green color
	setColor(Qt::green, Qt::darkRed);
}

TryToggle::~TryToggle()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void TryToggle::configure(const QString offText, const QString goingOnText, const QString onText, const QString goingOffText, const QColor onColor, const QColor goingOffColor, const quint32 blinkInterval)
{
	OC_METHODGATE();
	mOffText=offText;
	mGoingOnText=goingOnText;
	mOnText=onText;
	mGoingOffText=goingOffText;
	if(onColor.isValid()) {
		setColor(onColor, goingOffColor);
	}
	mTimer.setInterval(static_cast<int>(blinkInterval)); //Multiple of 60Hz gives stutter free animation
	updateText();
}


void TryToggle::setColor(const QColor onColor, const QColor goingOffColor)
{
	OC_METHODGATE();
	mOnPalette=mOffPalette;
	if(onColor.isValid()) {
		mOnPalette.setColor(QPalette::Button, onColor);
	}
	if(goingOffColor.isValid()) {
		mGoingOffPalette.setColor(QPalette::Button, goingOffColor);
	}
	ui->pushButtonToggle->setPalette(ON==mState?mOnPalette:mOffPalette);
}


TryToggleState TryToggle::state() const
{
	OC_METHODGATE();
	return mState;
}


bool TryToggle::isPositive()
{
	OC_METHODGATE();
	return ::positive(mState);
}


bool TryToggle::isTransitive()
{
	OC_METHODGATE();
	return ::transient(mState);

}


void TryToggle::updateText()
{
	OC_METHODGATE();
	QString t="";
	switch(mState) {

	case(OFF):
		t=mOffText;
		break;
	case(GOING_ON):
		t=mGoingOnText;
		break;
	case(ON):
		t=mOnText;
		break;
	case(GOING_OFF):
		t=mGoingOffText;
		break;

		//default:			t="ERROR";			break;
	}
	//qDebug()<<"TRYTOGGLE TEXT CHANGED TO "<<t;
	ui->pushButtonToggle->setText(t);
}


void TryToggle::setState(const TryToggleState nextState, const bool doEmit)
{
	OC_METHODGATE();
	qDebug()<<"TRYTOGGLE TRYSTATE CHANGING FROM "<<mState<<" TO "<<nextState<<" WITH EMIT="<<doEmit;
	//qDebug()<<"SET STATE: "<<s;
	if(nextState != mState) {
		//qDebug()<<" + DIFFERENT FROM LAST: "<<state;
		// Classify new state
		const bool isPositive=::positive(nextState) ;
		const bool isTransient=::transient(nextState);
		// Enable blinking if this is a transitioning state
		if(isTransient) {
			//qDebug()<<"TRYTOGGLE TRYSTATE TIMER START"<<" AND THIS="<<this;
			mTimer.start();
		} else {
			//qDebug()<<"TRYTOGGLE TRYSTATE TIMER STOP"<<" AND THIS="<<this;
			mTimer.stop();
		}
		//qDebug()<<"TRYTOGGLE TRYSTATE NOTOFF WAS "<<notOff<<" AND THIS="<<this;
		// Do we need to change the button state?
		if(isPositive != ui->pushButtonToggle->isChecked()) {
			//qDebug()<<"TRYTOGGLE TRYSTATE CHANGING BUTTON"<<" AND THIS="<<this;
			// Prevent calling ourselves
			const bool oldBlock=ui->pushButtonToggle->blockSignals(true);
			ui->pushButtonToggle->setChecked(isPositive);
			ui->pushButtonToggle->blockSignals(oldBlock);
		}
		const TryToggleState last = mState;
		mState = nextState;
		updateText();
		if(!isTransient) {
			ui->pushButtonToggle->setPalette(isPositive?mOnPalette:mOffPalette);
		}
		//qDebug()<<"TRYTOGGLE TRYSTATE CHANGED ACTUAL FROM "<<last<<" TO "<<mState <<" WITH EMIT="<<doEmit << " AND WITH TEXT= "<<ui->pushButtonToggle->text()<<" AND THIS="<<this;
		if(doEmit) {
			//qDebug()<<"Emitting " <<state;
			emit stateChanged(last, mState);
		}
	}
}


void TryToggle::animateClick()
{
	OC_METHODGATE();
	qDebug()<<" A N I M A T E   C L I C K ";
	ui->pushButtonToggle->animateClick();
}


void TryToggle::on_pushButtonToggle_toggled(bool checked)
{
	OC_METHODGATE();
	// Classify state
	const bool isPositive=::positive(mState) ;
	//qDebug()<<" X X X X  Clicked " <<checked;
	if(checked != isPositive) {
		setState(checked ? GOING_ON:GOING_OFF);
	}
}


void TryToggle::onTimeout()
{
	OC_METHODGATE();
	ui->pushButtonToggle->setPalette(mTimerToggle?(isPositive()?mOnPalette:mGoingOffPalette):mOffPalette);
	mTimerToggle=!mTimerToggle;
}


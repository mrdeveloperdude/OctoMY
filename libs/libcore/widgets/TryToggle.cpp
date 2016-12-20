#include "TryToggle.hpp"
#include "ui_TryToggle.h"

#include "../libutil/utility/Standard.hpp"


TryToggle::TryToggle(QWidget *parent) :
	QWidget(parent)
	, mState(OFF)
	, ui(new Ui::TryToggle)
{
	ui->setupUi(this);
	mTimer.setInterval(100);
	mTimer.setTimerType(Qt::PreciseTimer);
	connect(&mTimer,SIGNAL(timeout()),this,SLOT(onTimeout()));
	qRegisterMetaType<TryToggleState>("TryToggleState");
}

TryToggle::~TryToggle()
{
	delete ui;
}

void TryToggle::configure(const QString &t1i, const QString &t2i, const QString &t3i, const QColor &color)
{
	mT1=t1i;
	mT2=t2i;
	mT3=t3i;
	if(color.isValid()) {
		setColor(color);
	}
	updateText();
}

TryToggleState TryToggle::state() const
{
	return mState;
}

void TryToggle::setColor(const QColor &color)
{
	QPalette p=ui->pushButtonToggle->palette();
	p.setColor(QPalette::Button, color);
	ui->pushButtonToggle->setPalette(p);
}

void TryToggle::updateText()
{
	QString t="";
	switch(mState) {
	default:
		t="ERROR";
		break;
	case(OFF):
		t=mT1;
		break;
	case(TRYING):
		t=mT2;
		break;
	case(ON):
		t=mT3;
		break;
	}
	ui->pushButtonToggle->setText(t);
}

void TryToggle::setState(const TryToggleState s, const bool doEmit)
{
	OC_METHODGATE();
	//qDebug()<<"TRYTOGGLE TRYSTATE CHANGED TO: "<<s<<" WITH EMIT="<<doEmit<<" AND THIS="<<this;
	//qDebug()<<"SET STATE: "<<s;
	if(s!=mState) {
		//qDebug()<<" + DIFFERENT FROM LAST: "<<state;
		if(TRYING==s) {
			//qDebug()<<"TRYTOGGLE TRYSTATE TIMER START"<<" AND THIS="<<this;
			mTimer.start();
		} else {
			//qDebug()<<"TRYTOGGLE TRYSTATE TIMER STOP"<<" AND THIS="<<this;
			mTimer.stop();
		}
		const bool notOff=(OFF!=s);
		//qDebug()<<"TRYTOGGLE TRYSTATE NOTOFF WAS "<<notOff<<" AND THIS="<<this;
		ui->widgetLight->setLightOn(notOff);
		if(notOff!=ui->pushButtonToggle->isChecked()) {
			//qDebug()<<"TRYTOGGLE TRYSTATE CHANGING BUTTON"<<" AND THIS="<<this;
			// Prevent calling ourselves
			const bool old=ui->pushButtonToggle->blockSignals(true);
			ui->pushButtonToggle->setChecked(notOff);
			ui->pushButtonToggle->blockSignals(old);
		}
		const TryToggleState last=mState;
		mState=s;
		updateText();
		//qDebug()<<"TRYTOGGLE TRYSTATE CHANGED ACTUAL FROM "<<last<<" TO "<<mState <<" WITH EMIT="<<doEmit << " AND WITH TEXT= "<<ui->pushButtonToggle->text()<<" AND THIS="<<this;
		if(doEmit) {
			//qDebug()<<"Emitting " <<state;
			emit stateChanged(last,mState);
		}
	}
}

void TryToggle::animateClick()
{
	qDebug()<<" A N I M A T E   C L I C K ";
	ui->pushButtonToggle->animateClick();
}

void TryToggle::on_pushButtonToggle_toggled(bool checked)
{
	//qDebug()<<" X X X X  Clicked " <<checked;
	if(checked && OFF==mState) {
		setState(TRYING);
	} else if(!checked) {
		setState(OFF);
	}
}

void TryToggle::onTimeout()
{
	ui->widgetLight->toggleLight();
}



QDebug operator<<(QDebug d, const TryToggleState &s)
{
	QString str="";
	switch(s) {
	default:
		str="UNKNOWN";
		break;
	case(OFF):
		str="OFF";
		break;
	case(TRYING):
		str="TRYING";
		break;
	case(ON):
		str="ON";
		break;
	}

	d.nospace() << str;
	return d.space();
}


QString ToggleStateToSTring(TryToggleState s)
{
#define  ToggleStateToStringCASE(A) case (A):return #A
	switch(s) {
	default:
		return "UNKNOWN";
		ToggleStateToStringCASE(TRYING);
		ToggleStateToStringCASE(OFF);
		ToggleStateToStringCASE(ON);
	}
#undef ToggleStateToStringCASE
}

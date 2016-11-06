#include "TryToggle.hpp"
#include "ui_TryToggle.h"

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

TryToggle::~TryToggle(){
	delete ui;
}

void TryToggle::setText(QString t1i,QString t2i,QString t3i){
	mT1=t1i;
	mT2=t2i;
	mT3=t3i;
	updateText();
}

TryToggleState TryToggle::state()
{
	return mState;
}

void TryToggle::updateText(){
	QString t="";
	switch(mState){
		default:t="ERROR";  break;
		case(OFF):t=mT1;  break;
		case(TRYING):t=mT2;  break;
		case(ON):t=mT3;  break;
	}
	ui->pushButtonToggle->setText(t);
}

void TryToggle::setState(const TryToggleState s){
	//qDebug()<<"SET STATE: "<<s;
	if(s!=mState){
		//qDebug()<<" + DIFFERENT FROM LAST: "<<state;
		if(TRYING==s){
			mTimer.start();
		}
		else{
			mTimer.stop();
		}
		ui->widgetLight->setLightOn(!(OFF==s));
		ui->pushButtonToggle->setChecked(!(OFF==s));
		mState=s;
		updateText();
		//qDebug()<<"Emitting " <<state;
		emit stateChanged(s);
	}
}

void TryToggle::animateClick(){
	ui->pushButtonToggle->animateClick();
}

void TryToggle::on_pushButtonToggle_toggled(bool checked){
	//qDebug()<<"Clicked " <<checked;
	if(checked && OFF==mState){
		setState(TRYING);
	}
	else if(!checked){
		setState(OFF);
	}
}

void TryToggle::onTimeout(){
	ui->widgetLight->toggleLight();
}



QDebug operator<<(QDebug d, const TryToggleState &s){
	QString str="";
	switch(s){
		default:str="UNKNOWN";  break;
		case(OFF):str="OFF";  break;
		case(TRYING):str="TRYING";  break;
		case(ON):str="ON";  break;
	}

	d.nospace() << str;
	return d.space();
}


QString ToggleStateToSTring(TryToggleState s){
#define  ToggleStateToStringCASE(A) case (A):return #A
	switch(s){
		default:return "UNKNOWN";
			ToggleStateToStringCASE(TRYING);
			ToggleStateToStringCASE(OFF);
			ToggleStateToStringCASE(ON);
	}
#undef ToggleStateToStringCASE
}

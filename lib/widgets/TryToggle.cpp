#include "TryToggle.hpp"
#include "ui_TryToggle.h"

TryToggle::TryToggle(QWidget *parent) :
	QWidget(parent)
  , state(OFF)
  , ui(new Ui::TryToggle)
{
	ui->setupUi(this);
	timer.setInterval(100);
	timer.setTimerType(Qt::PreciseTimer);
	connect(&timer,SIGNAL(timeout()),this,SLOT(onTimeout()));
	qRegisterMetaType<TryToggleState>("TryToggleState");
}

TryToggle::~TryToggle(){
	delete ui;
}

void TryToggle::setText(QString t1i,QString t2i,QString t3i){
	t1=t1i;
	t2=t2i;
	t3=t3i;
	updateText();
}

void TryToggle::updateText(){
	QString t="";
	switch(state){
		default:t="ERROR";  break;
		case(OFF):t=t1;  break;
		case(TRYING):t=t2;  break;
		case(ON):t=t3;  break;
	}
	ui->pushButtonToggle->setText(t);
}

void TryToggle::setState(const TryToggleState s){
	qDebug()<<"SET STATE: "<<s;
	if(s!=state){
		//qDebug()<<" + DIFFERENT FROM LAST: "<<state;
		if(TRYING==s){
			timer.start();
		}
		else{
			timer.stop();
		}
		ui->widgetLight->setLightOn(!(OFF==s));
		ui->pushButtonToggle->setChecked(!(OFF==s));
		state=s;
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
	if(checked && OFF==state){
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

#include "HexyTool.hpp"
#include "ui_HexyTool.h"

HexyTool::HexyTool(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::HexyTool)
	, hexy(new HexySerial(this))
{
	ui->setupUi(this);
	const int pad=150;
	ui->numberEntryServoPosition->configure(500-pad,2500+pad,50,1500," µs","Servo position","servo_position");
	ui->tryToggleConnect->setText("Connect","Connecting","Connected");
	connect(hexy,SIGNAL(settingsChanged()), this, SLOT(onHexySettingsChanged()));
	connect(ui->tryToggleConnect, SIGNAL(stateChanged(TryToggleState)), this, SLOT(onConnectChanged(TryToggleState)));
	connect(ui->numberEntryServoPosition,SIGNAL(valueChanged(int)),this,SLOT(onServoPositionChanged()));
}

HexyTool::~HexyTool(){
	delete ui;
}


void HexyTool::onConnectChanged(TryToggleState s){
	ui->tabWidget->setEnabled(ON==s);
	switch(s) {
		case(OFF):{

			}break;
		case(TRYING):{
				setEnabled(false);
				hexy->configure();

			}break;
		case(ON):{

			}break;
	}

}


void HexyTool::onHexySettingsChanged(){
	ui->tryToggleConnect->setState(hexy->isConnected()?ON:OFF);
	setEnabled(true);
}


void HexyTool::onServoPositionChanged(){
	qreal val=ui->numberEntryServoPosition->value();
	qreal min=ui->numberEntryServoPosition->minimum();
	qreal max=ui->numberEntryServoPosition->maximum();
	val-=min;
	val/=(max-min);
	pos[0]=(val*2.0)-1.0;
	hexy->move(pos,0b00000000000000000000000000000001);
}

void HexyTool::on_pushButtonCenter_clicked(){
	ui->numberEntryServoPosition->setValue((ui->numberEntryServoPosition->minimum()+ui->numberEntryServoPosition->maximum())/2);
}

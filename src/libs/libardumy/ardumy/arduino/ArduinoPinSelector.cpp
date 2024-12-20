#include "ArduinoPinSelector.hpp"
#include "ui_ArduinoPinSelector.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include "ArduinoPinFacilitator.hpp"


ArduinoPinSelector::ArduinoPinSelector(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::ArduinoPinSelector)
	, mAPF(nullptr)
	, mPinMap( ArduinoPinMap::pinMapFactory("mega2560") )
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->labelDescription->setText("Not connected");
	QStringList pins=mPinMap.pinStringList();
	//for(QString str:pins) {		qDebug()<<"PIN: "<<str;	}
	ui->comboBoxPin->addItems(pins);
	adjustSize();

}

ArduinoPinSelector::~ArduinoPinSelector()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


void ArduinoPinSelector::configure(ArduinoPinFilter filter, ArduinoPinFacilitator *apf)
{
	OC_METHODGATE();
	if(nullptr!=mAPF) {
		if(!disconnect(mAPF, &ArduinoPinFacilitator::pinChanged, this, &ArduinoPinSelector::onPinChanged)) {
			qWarning()<<"ERROR: Could not disconnect";
		}
	}
	mFilter=filter;
	mAPF=apf;
	if(nullptr!=mAPF) {
		if(!connect(mAPF, &ArduinoPinFacilitator::pinChanged, this, &ArduinoPinSelector::onPinChanged, OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect";
		}
	}
	onPinChanged();
}


void ArduinoPinSelector::onPinChanged()
{
	OC_METHODGATE();
	if(nullptr!=mAPF) {
		//mAPF->
	}
	const ArduinoPinType type=mFilter.type();
	bool digitalIn=false;
	bool digitalOut=false;
	bool analogIn=false;
	bool pwmOut=false;
	switch(type) {
	case(PIN_DIGITAL_INPUT): {
		digitalIn=true;
	}
	break;
	case(PIN_DIGITAL_OUTPUT): {
		digitalOut=true;
	}
	break;
	case(PIN_ANALOG_INPUT): {
		analogIn=true;
	}
	break;
	case(PIN_PWM_OUTPUT): {
		pwmOut=true;
	}
	break;

	}
	ui->widgetDigitalIn->setVisible(digitalIn);
	ui->widgetDigitalOut->setVisible(digitalOut);
	ui->widgetAnalogIn->setVisible(analogIn);
	ui->widgetPWM->setVisible(pwmOut);
	bool connected=false;

	if(nullptr!=mAPF) {
		ui->labelDescription->setText(mAPF->description());
		connected=mAPF->isConnected();
	} else {
		ui->labelDescription->setText("N/A");
	}
	ui->widgetConnected->setLightOn(connected);
}


ArduinoPin ArduinoPinSelector::currentSelectedPin()
{
	OC_METHODGATE();
	const auto idx=ui->comboBoxPin->currentIndex();
	return mPinMap.pin(idx);
}

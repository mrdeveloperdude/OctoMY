#include "SerialActivity.hpp"
#include "ui_SerialActivity.h"

#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"


SerialActivity::SerialActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::SerialActivity)
	, mConfigureHelper("SerialActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


SerialActivity::~SerialActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void SerialActivity::configure(QSharedPointer<Node> n){
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		auto s = mNode->settings();
		//log("SETTING UP SERIAL");
		ui->widgetSerialSettings->configure();
			
		SerialSettings defaults;
		defaults.setIntBaudRate(115200);
		defaults.dataBits=QSerialPort::Data8;
		
		ui->widgetSerialSettings->configure(false, defaults);
		// TODO: Add connect try toggle button
		
		//ui->tryToggleConnect->configure("Connect","Connecting...","Connected", "Disconnecting...", Constants::AGENT_CONNECT_BUTTON_COLOR, Constants::AGENT_DISCONNECT_COLOR);
		// TODO: Add log window for status
		if(!connect(ui->widgetSerialSettings, &SerialSettingsWidget::settingsChanged, this, &SerialActivity::onSerialSettingsChanged) ) {
			qWarning()<<"ERROR: Could not connect";
		}
	}
}


void SerialActivity::onSerialSettingsChanged(){
	qWarning() << "Serial settings changed";
}


void SerialActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}


void SerialActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}

#include "ConnectionStatusWidget.hpp"
#include "ui_ConnectionStatusWidget.h"

#include "app/Constants.hpp"
#include "discovery/DiscoveryMandate.hpp"
#include "log/LogStorage.hpp"
#include "node/Node.hpp"
#include "node/NodeWindow.hpp"
#include "uptime/MethodGate.hpp"
#include "utility/color/Color.hpp"
#include "utility/random/Random.hpp"

ConnectionStatusWidget::ConnectionStatusWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::ConnectionStatusWidget)
	, mConfigureHelper("ConnectionStatusWidget", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


ConnectionStatusWidget::~ConnectionStatusWidget()
{
	OC_METHODGATE();
	delete ui;
}


void ConnectionStatusWidget::log(const QString &text, LogType type) const{
	OC_METHODGATE();
	if(!mNode.isNull()){
		mNode->log(text, type);
	}
	else{
		qWarning() << "Could not log" << text << "no node";
	}
}



void ConnectionStatusWidget::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		ui->tryToggleConnect->configure("Connect", "Connecting...", "Connected", "Disconnecting...", Constants::AGENT_CONNECT_BUTTON_COLOR, Constants::AGENT_DISCONNECT_COLOR);
		if(!connect(ui->pushButtonPanic, &PanicButton::panicChanged, this, &ConnectionStatusWidget::togglePanic)){
			qWarning()<<"COULD NOT CONNECT PANIC BUTTON";
		}
		mNode = n;
		if(!mNode.isNull()){
			mNode->setHookColorSignals(*this, true);
		}
	}
}


void ConnectionStatusWidget::setPanic(bool p){
	OC_METHODGATE();
	emit panic(p);
}


bool ConnectionStatusWidget::connecting() const {
	OC_METHODGATE();
	return ui->tryToggleConnect->isPositive();
}


bool ConnectionStatusWidget::audioOn() const {
	OC_METHODGATE();
	return ui->pushButtonAudio->isChecked();
}


bool ConnectionStatusWidget::cameraOn() const {
	OC_METHODGATE();
	return ui->pushButtonAudio->isChecked();
}


bool ConnectionStatusWidget::bluetoothOn() const {
	OC_METHODGATE();
	return ui->pushButtonAudio->isChecked();
}


bool ConnectionStatusWidget::NFCOn() const {
	OC_METHODGATE();
	return ui->pushButtonAudio->isChecked();
}


bool ConnectionStatusWidget::networkOn() const {
	OC_METHODGATE();
	return ui->pushButtonAudio->isChecked();
}


bool ConnectionStatusWidget::zooOn() const {
	OC_METHODGATE();
	return ui->pushButtonAudio->isChecked();
}


bool ConnectionStatusWidget::panicOn() const {
	OC_METHODGATE();
	return ui->pushButtonAudio->isChecked();
}



void ConnectionStatusWidget::openSettings(){
	OC_METHODGATE();
	emit openConnectionSettings();
}

static int ss=0;
void ConnectionStatusWidget::randomizeColor()
{
	OC_METHODGATE();
	ss=(ss+1)%5;
	ui->widgetSignalStrength->signalStrengthChanged(ss-1);
	QPalette p = ui->pushButtonRandomizeColor->palette();
	QColor col;
	const auto hue = utility::random::frand();
	col.setHslF(hue, 1.0f, 0.5f);
	// qDebug() << "Red:" << col.red() << "Green:" << col.green() << "Blue:" << col.blue()<< "Hue:"<< hue;
	p.setColor(QPalette::Button, col);
	p.setColor(QPalette::ButtonText, utility::color::intensity(col) > 0.5f?Qt::black:Qt::white);
	ui->pushButtonRandomizeColor->setPalette(p);
	log(QString("New color created: %1").arg(col.name()));
	emit colorChanged(col);
}


void ConnectionStatusWidget::startCamera()
{
	OC_METHODGATE();
	qDebug()<<"CAM";
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(mNode){
			auto window = mNode->nodeWindow();
			if(window){
				log("Enabling camera pairing mode");
				window->pushPage("CameraPairingActivity");
			}
		}
	}
}

void ConnectionStatusWidget::toggleAudio(const bool on)
{
	OC_METHODGATE();
	qDebug()<<"AU";
	if(mConfigureHelper.isConfiguredAsExpected()) {
		log(QString("Toggling audio pairing %1").arg(on?"ON":"OFF"));
	}
}

void ConnectionStatusWidget::toggleBluetooth(const bool on)
{
	OC_METHODGATE();
	qDebug()<<"BT";
	if(mConfigureHelper.isConfiguredAsExpected()) {
		log(QString("Toggling bluetooth pairing %1").arg(on?"ON":"OFF"));
	}
}

void ConnectionStatusWidget::toggleNetwork(const bool on)
{
	OC_METHODGATE();
	qDebug()<<"NT";
	if(mConfigureHelper.isConfiguredAsExpected()) {
		log(QString("Toggling network pairing %1").arg(on?"ON":"OFF"));
	}
}


void ConnectionStatusWidget::toggleZoo(bool on){
	OC_METHODGATE();
	qDebug()<<"ZO";
	if(mConfigureHelper.isConfiguredAsExpected()) {
		log(QString("Toggling zoo pairing %1").arg(on?"ON":"OFF"));
	}
}


void ConnectionStatusWidget::togglePanic(bool panic)
{
	OC_METHODGATE();
	qDebug()<<"PANIC";
	QString str;
	if(mConfigureHelper.isConfiguredAsExpected()){
		ui->pushButtonPanic->setPanic(panic);
		if(!mNode.isNull()) {
			mNode->setPanic(panic);
			if(panic) {
				str="P A N I C !";
			} else {
				str="Panic averted";
			}
		}
		else{
			str="Panic could not be propegated :-|";
		}
		qWarning()<<str;
		log(str);
	}
}



QSharedPointer<DiscoveryMandate> ConnectionStatusWidget::discoveryMandate(){
	// TODO: Research viability of shared "ConnectionMandate" object that combines discovery mandate with connection status.
	return QSharedPointer<DiscoveryMandate>::create(connecting(), zooOn(), networkOn(), bluetoothOn(), audioOn(), NFCOn(), cameraOn());
}



#include "ConnectionStatusWidget.hpp"
#include "ui_ConnectionStatusWidget.h"

#include "app/Constants.hpp"
#include "comms/CommsSession.hpp"
#include "discovery/DiscoveryMandate.hpp"
#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"
#include "utility/color/Color.hpp"
#include "utility/random/Random.hpp"

ConnectionStatusWidget::ConnectionStatusWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::ConnectionStatusWidget)
	, mConfigureHelper("ConnectionStatusWidget", true, false, false, true, false)
{
	ui->setupUi(this);
}


ConnectionStatusWidget::~ConnectionStatusWidget()
{
	delete ui;
}


void ConnectionStatusWidget::log(const QString &text, LogType type) const{
	OC_METHODGATE();
	if(!mNode.isNull()){
		mNode->log(text, type);
	}
}


bool ConnectionStatusWidget::connecting() const {
	return ui->tryToggleConnect->isPositive();
}


bool ConnectionStatusWidget::audioOn() const {
	return ui->pushButtonAudio->isChecked();
}


bool ConnectionStatusWidget::cameraOn() const {
	return ui->pushButtonAudio->isChecked();
}


bool ConnectionStatusWidget::bluetoothOn() const {
	return ui->pushButtonAudio->isChecked();
}


bool ConnectionStatusWidget::NFCOn() const {
	return ui->pushButtonAudio->isChecked();
}


bool ConnectionStatusWidget::networkOn() const {
	return ui->pushButtonAudio->isChecked();
}


bool ConnectionStatusWidget::zooOn() const {
	return ui->pushButtonAudio->isChecked();
}


bool ConnectionStatusWidget::panicOn() const {
	return ui->pushButtonAudio->isChecked();
}


// An error occurred in comms
void ConnectionStatusWidget::onCommsError(QString message){
	OC_METHODGATE();
	Q_UNUSED(message);
}

// A new comms session was added
void ConnectionStatusWidget::onCommsClientAdded(CommsSession *c){
	OC_METHODGATE();
	auto address = c->address();
}

// The connection state changed for comms channel
void ConnectionStatusWidget::onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection){
	OC_METHODGATE();
	const auto tts = createTryToggleState(isConnected, needsConnection);
	ui->tryToggleConnect->setState(tts);
}



void ConnectionStatusWidget::toggleConnect(bool connect){
	emit tryConnect(connect);
}


void ConnectionStatusWidget::openSettings(){
	emit openConnectionSettings();
}

void ConnectionStatusWidget::randomizeColor()
{
	OC_METHODGATE();
	QPalette p = ui->pushButtonRandomizeColor->palette();
	QColor col;
	const auto hue = utility::random::frand();
	col.setHslF(hue, 1.0f, 0.5f);
	qDebug() << "Red:" << col.red() << "Green:" << col.green() << "Blue:" << col.blue()<< "Hue:"<< hue;
	p.setColor(QPalette::Button, col);
	p.setColor(QPalette::ButtonText, utility::color::intensity(col) > 0.5f?Qt::black:Qt::white);
	ui->pushButtonRandomizeColor->setPalette(p);
	log(QString("New color: %1").arg(col.name()));
	emit colorChanged(col);
}


void ConnectionStatusWidget::startCamera()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		//push("CameraPairingActivity");
	}
}

void ConnectionStatusWidget::toggleAudio(const bool on)
{
	OC_METHODGATE();
	log(QString("Toggling audio pairing %1").arg(on?"ON":"OFF"));
}

void ConnectionStatusWidget::toggleBluetooth(const bool on)
{
	OC_METHODGATE();
	log(QString("Toggling bluetooth pairing %1").arg(on?"ON":"OFF"));
}

void ConnectionStatusWidget::toggleNetwork(const bool on)
{
	OC_METHODGATE();
	log(QString("Toggling network pairing %1").arg(on?"ON":"OFF"));
}


void ConnectionStatusWidget::togglePanic(bool panic)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		ui->pushButtonPanic->setPanic(panic);
		if(!mNode.isNull()) {
			mNode->setPanic(panic);
			if(panic) {
				QString str="P A N I C !";
				qWarning()<<str;
				log(str);
			} else {
				QString str="Panic averted";
				qWarning()<<str;
				log(str);
			}
		}
		else{
			QString str="Panic could not be propegated :-|";
			qWarning()<<str;
			log(str);
		}
	}
}

void ConnectionStatusWidget::toggleZoo(bool on){
	OC_METHODGATE();
	log(QString("Toggling zoo pairing %1").arg(on?"ON":"OFF"));
}


QSharedPointer<DiscoveryMandate> ConnectionStatusWidget::discoveryMandate(){
	// TODO: Research viability of shared "ConnectionMandate" object that combines discovery mandate with connection status.
	return QSharedPointer<DiscoveryMandate>::create(connecting(), zooOn(), networkOn(), bluetoothOn(), audioOn(), NFCOn(), cameraOn());
}



void ConnectionStatusWidget::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		ui->tryToggleConnect->configure("Connect", "Connecting...", "Connected", "Disconnecting...", Constants::AGENT_CONNECT_BUTTON_COLOR, Constants::AGENT_DISCONNECT_COLOR);
		mNode = n;
		if(!mNode.isNull()){
			mNode->setHookCommsSignals(*this, true);
			mNode->setHookColorSignals(*this, true);
		}
	}
}

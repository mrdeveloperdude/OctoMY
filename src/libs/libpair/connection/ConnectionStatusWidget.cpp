#include "ConnectionStatusWidget.hpp"
#include "ui_ConnectionStatusWidget.h"

#include "log/LogStorage.hpp"
#include "app/Constants.hpp"
#include "comms/CommsSession.hpp"
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
	ui->tryToggleConnect->configure("Connect", "Connecting...", "Connected", "Disconnecting...", Constants::AGENT_CONNECT_BUTTON_COLOR, Constants::AGENT_DISCONNECT_COLOR);
}


ConnectionStatusWidget::~ConnectionStatusWidget()
{
	delete ui;
}


void ConnectionStatusWidget::appendLog(const QString &text){
	OC_METHODGATE();
	if(mNode){
		auto logStorage = mNode->logStorage();
		if(logStorage){
			logStorage->appendLog(text);
		}
	}
}



bool ConnectionStatusWidget::connecting() const{
	return ui->tryToggleConnect->isPositive();
}


void ConnectionStatusWidget::connectToggled(bool connect){
	emit tryConnect(connect);
}


void ConnectionStatusWidget::openSettings(){
	emit openConnectionSettings();
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
	appendLog(QString("New color: %1").arg(col.name()));
	emit colorChanged(col);
}



void ConnectionStatusWidget::panicChanged(bool panic)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		ui->pushButtonPanic->setPanic(panic);
		if(!mNode.isNull()) {
			mNode->setPanic(panic);
			if(panic) {
				QString str="P A N I C !";
				qWarning()<<str;
				appendLog(str);
			} else {
				QString str="Panic averted";
				qWarning()<<str;
				appendLog(str);
			}
		}
		else{
			QString str="Panic could not be propegated :-|";
			qWarning()<<str;
			appendLog(str);
		}
	}
}




void ConnectionStatusWidget::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		mNode = n;
		if(!mNode.isNull()){
			mNode->setHookCommsSignals(*this, true);
			mNode->setHookColorSignals(*this, true);
		}
		connect(ui->pushButtonPanic, &PanicButton::panicChanged, this, & ConnectionStatusWidget::panicChanged);
	}
}

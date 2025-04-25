#include "ControlUnboxingWizard.hpp"
#include "ui_ControlUnboxingWizard.h"

#include "app/Constants.hpp"
#include "discovery/AddressBook.hpp"
#include "node/Node.hpp"
#include "remote/RemoteWindow.hpp"
#include "security/KeyStore.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"


ControlUnboxingWizard::ControlUnboxingWizard(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::ControlUnboxingWizard)
	, mConfigureHelper("ControlUnboxingWizard", true, false, true)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


ControlUnboxingWizard::~ControlUnboxingWizard()
{
	OC_METHODGATE();
	delete ui;
}


bool ControlUnboxingWizard::isDelivered() const{
	OC_METHODGATE();
	auto keystore = mNode->keyStore();
	if(!keystore.isNull()) {
		auto key = keystore->localKey();
		if(!key.isNull()) {
			return true;
		}
		else {
			qWarning()<<"No local key";
		}
	}
	else {
		qWarning()<<"No keystore";
		
	}
	return false;
}


bool ControlUnboxingWizard::isPaired() const{
	OC_METHODGATE();
	QVector<AddressQueryRule> f;
	f.append(AddressQueryRule(TYPE_AGENT, false, true, true));
	auto result = mNode->addressBook()->filter(f);
	return result.size() > 0;
}



bool ControlUnboxingWizard::isPairingSkipped() const{
	OC_METHODGATE();
	auto s{mNode->settings()};
	if(!s){
		qWarning() << "No settings";
		return false;
	}
	return (s->getCustomSettingBool(Constants::SETTINGS_KEY_PAIRING_SKIPPED, false));
}



ControlUnboxingStage ControlUnboxingWizard::unboxingStage(){
	OC_METHODGATE();
	auto stage = UNKNOWN_STAGE;
	if(mConfigureHelper.isConfiguredAsExpected()){
		auto identity = mNode->nodeIdentity();
		if(!isDelivered()){
			stage = DELIVERY_STAGE;
		}
		else if(!isPaired() && !isPairingSkipped()){
			stage = PAIRING_STAGE;
		}
		else{
			stage = UNBOXING_COMPLETE;
		}
	}
	return stage;
}


void ControlUnboxingWizard::updateStage(){
	OC_METHODGATE();
	bool delivered{false};
	bool paired{false};
	bool pairingSkipped{false};
	bool allOK{false};
	QString name;
	ControlUnboxingStage stage{UNKNOWN_STAGE};
	PortableID id;
	QString text{"Next"};
	if(mConfigureHelper.isConfiguredAsExpected()){
		delivered = isDelivered();
		if(delivered){
			id = mNode->nodeIdentity()->toPortableID();
		}
		paired = isPaired();
		pairingSkipped = isPairingSkipped();
		stage = unboxingStage();
	}
	switch(stage){
		case(DELIVERY_STAGE): text = "Start unboxing";break;
		case(HANDOVER_STAGE): text = "Launch Remote!";break;
		default:break;
	}
	allOK = delivered && (paired || pairingSkipped);
	ui->widgetDeliveredIdenticon->setVisible(!id.id().isEmpty());
	ui->widgetDeliveredIdenticon->setPortableID(id);
	ui->lightWidgetDelivered->setLightOn(delivered);
	ui->lightWidgetPaired->setLightOn(paired || pairingSkipped);
	ui->lightWidgetPaired->setLightColor(paired?Qt::green:Qt::yellow);
	ui->lightWidgetAllDone->setLightOn(allOK);
	ui->lightWidgetAllDone->setLightColor(paired?Qt::green:Qt::yellow);
	ui->pushButtonNextStep->setText(text);
}


void ControlUnboxingWizard::nextStageClicked(){
	OC_METHODGATE();
	const auto stage = unboxingStage();
	qDebug()<<"CLICKED STAGE"<<stage;
	if(!mNode.isNull()) {
		bool pairing{false};
		switch(stage){
			case DELIVERY_STAGE: push("ControlDeliveryActivity"); break;
			case PAIRING_STAGE: push("PairingActivity"); pairing = true; break;
			case UNBOXING_COMPLETE: push("AgentSelectActivity"); break;
			case HANDOVER_STAGE: push("AgentSelectActivity"); break;
			default:
			case UNKNOWN_STAGE: push("ControlUnboxingWizard"); break;
		}
		mNode->discoveryActivate(pairing);
	}
}


bool ControlUnboxingWizard::unboxingDone(){
	OC_METHODGATE();
	return unboxingStage() == UNBOXING_COMPLETE;
}


void ControlUnboxingWizard::configure(QSharedPointer<Node> node){
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		mNode = node;
		if(!mNode.isNull()) {
			if(!connect(mNode.data(), &Node::identityChanged, this, [this]() {
					updateStage();
				},OC_CONTYPE_NON_UNIQUE)) {
				qWarning()<<"ERROR: Could not connect";
			}
		}
		updateStage();
	}
}


void ControlUnboxingWizard::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
}


void ControlUnboxingWizard::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	qDebug() << "control unboxing pop args" << returnActivity << returnArguments;
	if("MessageActivity" == returnActivity){
		if(returnArguments.size() >=2){
			const auto id = returnArguments[0];
			const auto response = returnArguments[1];
			if("quit" == id){
				if("true" == response){
					auto window = mNode->nodeWindow();
					if(window){
						// TODO: refactor activity system into nodewindow
						// window->applicationShutdown();
					}
					else{
						qWarning()<<"No remote window";
					}
				}
			}
		}
	}
}

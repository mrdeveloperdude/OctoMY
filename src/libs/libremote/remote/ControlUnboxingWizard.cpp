#include "ControlUnboxingWizard.hpp"
#include "remote/Remote.hpp"
#include "remote/RemoteWindow.hpp"
#include "ui_ControlUnboxingWizard.h"

#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"

ControlUnboxingWizard::ControlUnboxingWizard(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::ControlUnboxingWizard)
	, mConfigureHelper("ControlUnboxingWizard", true, false, true)
{
	ui->setupUi(this);
}


ControlUnboxingWizard::~ControlUnboxingWizard()
{
	delete ui;
}


bool ControlUnboxingWizard::isDelivered() const{
	auto keystore = mRemote->keyStore();
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


bool ControlUnboxingWizard::isPaierd() const{
	QVector<QueryRule> f;
	f.append(QueryRule(TYPE_AGENT, false, true, true));
	auto result = mRemote->addressBook()->filter(f);
	return result.size() > 0;
}


ControlUnboxingStage ControlUnboxingWizard::unboxingStage(){
	OC_METHODGATE();
	auto stage = UNKNOWN_STAGE;
	if(mConfigureHelper.isConfiguredAsExpected()){
		auto identity = mRemote->nodeIdentity();
		if(!isDelivered()){
			stage = DELIVERY_STAGE;
		}
		else if(!isPaierd()){
			stage = PAIRING_STAGE;
		}
		else{
			stage = HANDOVER_STAGE;
		}
	}
	return stage;
}


void ControlUnboxingWizard::updateStage(){
	bool delivered{false};
	bool paired{false};
	bool allOK{false};
	QString name;
	ControlUnboxingStage stage{UNKNOWN_STAGE};
	PortableID id;
	QString text{"Next"};
	if(mConfigureHelper.isConfiguredAsExpected()){
		delivered = isDelivered();
		if(delivered){
			id = mRemote->nodeIdentity()->toPortableID();
		}
		paired = isPaierd();
		stage = unboxingStage();
	}
	switch(stage){
		case(DELIVERY_STAGE): text = "Start unboxing";break;
		case(HANDOVER_STAGE): text = "Launch Remote!";break;
		default:break;
	}
	allOK = delivered && paired;
	ui->widgetDeliveredIdenticon->setVisible(!id.id().isEmpty());
	ui->widgetDeliveredIdenticon->setPortableID(id);
	ui->lightWidgetDelivered->setLightOn(delivered);
	ui->lightWidgetPaired->setLightOn(paired);
	ui->lightWidgetAllDone->setLightOn(allOK);
	ui->pushButtonNextStep->setText(text);
}


void ControlUnboxingWizard::nextStageClicked(){
	const auto stage = unboxingStage();
	qDebug()<<"CLICKED STAGE"<<stage;
	if(!mRemote.isNull()) {
		bool pairing{false};
		switch(stage){
			case DELIVERY_STAGE: push("ControlDeliveryActivity"); break;
			case PAIRING_STAGE: push("PairingActivity"); pairing = true; break;
			case UNBOXING_COMPLETE: push("AgentSelectActivity"); break;
			case HANDOVER_STAGE: push("AgentSelectActivity"); break;
			default:
			case UNKNOWN_STAGE: push("ControlUnboxingWizard"); break;
		}
		mRemote->discoveryActivate(pairing);
	}
}


bool ControlUnboxingWizard::unboxingDone(){
	return unboxingStage() == UNBOXING_COMPLETE;
}


void ControlUnboxingWizard::configure(QSharedPointer<Remote> remote){
	if(mConfigureHelper.configure()){
		mRemote = remote;
		if(!mRemote.isNull()) {
			if(!connect(mRemote.data(), &Node::identityChanged, this, [this]() {
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
					auto window = qSharedPointerCast<RemoteWindow>(mRemote->nodeWindow());
					if(window){
						window->applicationShutdown();
					}
					else{
						qWarning()<<"No remote window";
					}
				}
			}
		}
	}
}

#include "PairingActivity.hpp"
#include "ui_PairingActivity.h"

#include "address/Associate.hpp"
#include "discovery/DiscoveryClient.hpp"
#include "node/Node.hpp"
#include "pairing/list/PairingListModel.hpp"
#include "security/PortableID.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QDebug>
#include <QDesktopServices>
#include <QItemDelegate>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QRegularExpression>
#include <QSvgRenderer>
#include <QUrl>
#include <QWidget>

// QDesktopServices::openUrl(QUrl("https://www.youtube.com/watch?v=mTiqZm-Ea70", QUrl::TolerantMode));


const QString PairingActivity::mEncouragmentTextControl{R"(
# No Agents
You may find that using your **remote** or **hub** without an **agent** to control can be unfulfilling. That is why we *strongly* encourage you to set up at least one **agent**.
You can set up your agents at any time and pair them with this control whenever you are ready.
)"};

const QString PairingActivity::mEncouragmentTextAgent{R"(
# No Controls
You may find that using your **agent** without a **remote** or **hub** to control it can be unfulfilling. That is why we *strongly* encourage you to set up at least one **control**.
You can set up your controls at any time and pair it with this agent whenever you are ready.
)"};




PairingActivity::PairingActivity(QWidget *parent)
	: Activity(parent)
	, ui(OC_NEW Ui::PairingActivity)
	, mConfigureHelper("PairingActivity", true, false, false, true, false)

{
	OC_METHODGATE();
	ui->setupUi(this);
}


PairingActivity::~PairingActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}




void PairingActivity::replaceText(NodeType type)
{
	OC_METHODGATE();
	switch(type) {
	default:
	case(TYPE_ZOO):
	case(TYPE_UNKNOWN):
	case(TYPE_AGENT): {
		ui->labelBodyPair->setText(mTemplate.replace(QRegularExpression("\\[SOURCE\\]"), "Agent").replace(QRegularExpression("\\[DEST\\]"), "Control"));
	}
	break;
	case(TYPE_REMOTE): {
		ui->labelBodyPair->setText(mTemplate.replace(QRegularExpression("\\[SOURCE\\]"), "Remote").replace(QRegularExpression("\\[DEST\\]"), "Agent"));
	}
	break;
	case(TYPE_HUB): {
		ui->labelBodyPair->setText(mTemplate.replace(QRegularExpression("\\[SOURCE\\]"), "Hub").replace(QRegularExpression("\\[DEST\\]"), "Agent"));
	}
	break;
	}
}



void PairingActivity::hookStartEdit()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(mDebug){
			qDebug()<<"EDIT HOOKED";
		}
		ui->widgetPairingList->configure(mNode);
		if(!connect(ui->widgetPairingList, &PairingListWidget::startEdit, this, [this](const QString id) {
				if(mDebug){
					qDebug()<<"Start EDIT";
				}
				const auto addressBook = mNode->addressBook();
				if(!addressBook.isNull()) {
					const auto ass = addressBook->associateByID(id);
					if(!ass.isNull()) {
						startEdit(id);
					} else {
						qWarning()<<"ERROR: No ass";
					}
				}
			}, OC_CONTYPE_NON_UNIQUE)) {
			qWarning()<<"ERROR: Could not connect";
		}
	}
}


void PairingActivity::updateDiscoveryMandate(){
	if(!mDiscoveryClient.isNull()){
		mDiscoveryClient->setMandate(ui->widgetConnectionStatus->discoveryMandate());
	}
	else{
		qWarning()<<"No discovey client while updating mandate";
	}
}


void PairingActivity::log(const QString &text, LogType type) const{
	OC_METHODGATE();
	if(!mNode.isNull()){
		mNode->log(text, type);
	}
}

void PairingActivity::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		ui->widgetMyCertificate->configure(this, false, true);
		mTemplate = ui->labelBodyPair->text();
		ui->labelBodyPair->setText("<h1>Please wait...</h1>");
		hookStartEdit();
		auto lal = localAddressList();
		if(!mNode.isNull() && !lal.isNull()) {
			const auto type = mNode->nodeType();
			const auto ass = mNode->nodeIdentity();
			ui->widgetConnectionStatus->configure(mNode);
			if(!ass.isNull()) {
				auto pid = ass->toPortableID();
				//qDebug()<<"CONFIGURE PAIRING WIZ FOR "<<pid.toPortableString();
				ui->widgetMyCertificate->setPortableID(pid);
				auto client = mNode->discoveryClient();
				if(!client.isNull()) {
					if(!connect(client.data(), &DiscoveryClient::nodeDiscovered, [=](QString partID) {
							Q_UNUSED(partID);
							//qDebug()<<"PAIRING WIZARD partID: "<<partID;
							//ui->listViewNodes->update();
						}
								 )) {
						qWarning()<<"ERROR: Could not connect "<<client->objectName();
					}
				} else {
					qWarning()<<"ERROR: discovery was null";
					return;
				}
				
				replaceText(type);
				const auto carrierAddress = lal->currentCarrierAddress();
				// TODO: Adapt to use CarrierAddress
				/*
				if(!connect(ui->widgetConnectionStatus, &ConnectionStatusWidget::colorChanged, this, &PairingActivity::onNetworkSettingsChange, OC_CONTYPE)) {
					qWarning()<<"ERROR: Could not connect "<<ui->widgetNetworkSettings->objectName();
				}
				
				
				ui->widgetNetworkSettings->setCarrierAddress(carrierAddress);
				*/
				
			} else {
				qWarning()<<"ERROR: No local ass";
			}
			mDiscoveryClient = mNode->discoveryClient();
			if(!mDiscoveryClient.isNull()){
				connect(mDiscoveryClient.get(), &DiscoveryClient::discoverRequest, this, [=](){
					qDebug() << "PING";
					ui->widgetPings->ping(0xFF66FF22, true);
				});
				connect(mDiscoveryClient.get(), &DiscoveryClient::discoverResponse, this, [=](const bool ok){
					qDebug() << "PONG" << ok;
					ui->widgetPings->ping(ok?0xFF2266FF:0xFFFF0000, false);
				});
			}
			else {
				qWarning()<<"ERROR: No discovery client";
			}
			
		} else {
			qWarning()<<"ERROR: No node";
		}
	}
}


void PairingActivity::onNetworkSettingsChange(QHostAddress address, quint16 port, bool valid)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(valid) {
			auto lal=localAddressList();
			if(!lal.isNull()) {
				lal->setCurrent(address, port);
			}
		}
		updateNetworkSettings();
	}
}


void PairingActivity::updateNetworkSettings()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mNode.isNull()) {
			auto client=mNode->discoveryClient();
			if(!client.isNull()) {
				const bool visible=this->isVisible();
				//TODO: Only attemt to start discovery client when address is valid
				//const bool valid=true;//mNode->localAddresses().currentAddress().isValid()
				client->activate(visible);
				ui->progressBarSearching->setVisible(visible);
			}
		}
	}
}


QSharedPointer<AddressBook> PairingActivity::addressBook()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(nullptr!=mNode) {
			return mNode->addressBook();
		}
	}
	return nullptr;
}


QSharedPointer<LocalAddressList> PairingActivity::localAddressList()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(nullptr!=mNode) {
			return mNode->localAddressList();
		}
	}
	return nullptr;
}


bool PairingActivity::isPaierd() const{
	QVector<QueryRule> f;
	switch(mNode->nodeRole()){
	case ROLE_AGENT:
		f.append(QueryRule(TYPE_REMOTE, false, true, true));
		f.append(QueryRule(TYPE_HUB, false, true, true));
		break;
	case ROLE_CONTROL:
		f.append(QueryRule(TYPE_AGENT, false, true, true));
		break;
	default:
	case ROLE_UNKNOWN:
		return false;
	}
	auto result = mNode->addressBook()->filter(f);
	return result.size() > 0;
}


void PairingActivity::startEdit(const QString &id)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected("startEdit")) {
		if(mDebug){
			qDebug() << "STARTING EDIT FOR " << id;
		}
		mCurrentlyEditingID = id;
		push("PairingTrustActivity", QStringList() << mCurrentlyEditingID);
	}
}


QSharedPointer<Node> PairingActivity::node()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		return mNode;
	}
	return nullptr;
}


void PairingActivity::showEvent(QShowEvent *)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		updateNetworkSettings();
	}
}


void PairingActivity::hideEvent(QHideEvent *)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		updateNetworkSettings();
	}
}


void PairingActivity::complete()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(isPaierd()){
			pop();
		}
		else{
			switch(mNode->nodeRole()){
			case ROLE_AGENT:
				push("MessageActivity", QStringList() << "encouragement" << "" << mEncouragmentTextAgent << ":/icons/app/warning.svg" << "true");
				break;
			case ROLE_CONTROL:
				push("MessageActivity", QStringList() << "encouragement" << "" << mEncouragmentTextControl << ":/icons/app/warning.svg" << "true");
				break;
			default:
			case ROLE_UNKNOWN:
				pop();
			}
		}
	}
}


void PairingActivity::refresh()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(nullptr != mList) {
			qDebug() << mList->status();
		} else {
			qWarning() << "No list";
		}
	}
}


void PairingActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if("MessageActivity" == returnActivity){
			if(returnArguments.size() != 2){
				qWarning() << "Invalid number of return args in pop";
				return;
			}
			auto id = returnArguments[0];
			if("encouragement" == id){
				auto doneString = returnArguments[1];
				if("true" == doneString){
					auto s{mNode->settings()};
					if(s){
						s->setCustomSettingBool(Constants::SETTINGS_KEY_PAIRING_SKIPPED, true);
					}
					pop();
				}
			}
			return;
		}
		if("PairingTrustActivity" == returnActivity){
			if(returnArguments.size() != 1){
				qWarning() << "Invalid number of return args in pop";
				return;
			}
			auto levelString = returnArguments[0];
			auto level = trustLevelFromString(levelString);
			qDebug() << "SAVING AFTER EDIT OF " << mCurrentlyEditingID;
			auto peers = addressBook();
			if(!mNode.isNull() && !peers.isNull()) {
				if(REMOVE == level){
					peers->removeAssociate(mCurrentlyEditingID);
				}
				else{
					auto peer = peers->associateByID(mCurrentlyEditingID);
					if(peer){
						NodeType type = peer->type();
						peer->trusts().applyTrustLevel(level, type);
						qDebug() << "EDITING ENDS WITH trusts: " << peer->trusts().toStringList();
						peers->save();
					}
					else{
						qWarning() << "ERROR: No peer for " << mCurrentlyEditingID;
					}
				}
				//ui->listViewNodes->update();
			} else {
				qWarning() << "ERROR: No node or peers while saving trust edits";
			}
			mCurrentlyEditingID = "";
			return;
		}
	}
}


void PairingActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(arguments.size() == 1){
			auto id = arguments[0];
		}
	}
}


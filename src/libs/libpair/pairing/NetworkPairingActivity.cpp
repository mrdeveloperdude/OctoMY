#include "NetworkPairingActivity.hpp"
#include "ui_NetworkPairingActivity.h"

#include "PairingEditButtonDelegate.hpp"
#include "address/Associate.hpp"
#include "comms/address/NetworkAddress.hpp"
#include "discovery/DiscoveryClient.hpp"
#include "node/Node.hpp"
#include "pairing/PairingListModel.hpp"
#include "security/PortableID.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QDebug>
#include <QRegularExpression>
#include <QDesktopServices>
#include <QUrl>

#include <QWidget>
#include <QItemDelegate>
#include <QMouseEvent>
#include <QPainter>
#include <QSvgRenderer>
#include <QMessageBox>

// QDesktopServices::openUrl(QUrl("https://www.youtube.com/watch?v=mTiqZm-Ea70", QUrl::TolerantMode));


const QString NetworkPairingActivity::mEncouragmentTextControl{R"(
# No Agents
You may find that using your **remote** or **hub** without an **agent** to control can be unfulfilling. That is why we *strongly* encourage you to set up at least one **agent**.
You can set up your agents at any time and pair them with this control whenever you are ready.
)"};

const QString NetworkPairingActivity::mEncouragmentTextAgent{R"(
# No Controls
You may find that using your **agent** without a **remote** or **hub** to control it can be unfulfilling. That is why we *strongly* encourage you to set up at least one **control**.
You can set up your controls at any time and pair it with this agent whenever you are ready.
)"};





NetworkPairingActivity::NetworkPairingActivity(QWidget *parent)
	: Activity(parent)
	, ui(OC_NEW Ui::NetworkPairingActivity)
	, mConfigureHelper("NetworkPairingActivity", true, false, false, true, false)
	
{
	OC_METHODGATE();
	ui->setupUi(this);
}


NetworkPairingActivity::~NetworkPairingActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}



void NetworkPairingActivity::replaceText(NodeType type)
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



void NetworkPairingActivity::hookStartEdit()
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


void NetworkPairingActivity::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		ui->widgetMyCertificate->configure(false, true);
		mTemplate = ui->labelBodyPair->text();
		ui->labelBodyPair->setText("<h1>Please wait...</h1>");
		hookStartEdit();
		auto lal = localAddressList();
		if(!mNode.isNull() && !lal.isNull()) {
			const auto type = mNode->nodeType();
			const auto ass = mNode->nodeIdentity();
			if(!ass.isNull()) {
				auto pid = ass->toPortableID();
				//qDebug()<<"CONFIGURE PAIRING WIZ FOR "<<pid.toPortableString();
				ui->widgetMyCertificate->setPortableID(pid);
				if(nullptr == ui->listViewNodes->model()) {
					if(nullptr == mList) {
						mList = OC_NEW PairingListModel(addressBook(), type);
					}
					ui->listViewNodes->setModel(mList);
					if(nullptr == mDelegate) {
						mDelegate = OC_NEW PairingEditButtonDelegate(this);
					}
					ui->listViewNodes->setItemDelegate(mDelegate);
				}
				auto client = mNode->discoveryClient();
				if(!client.isNull()) {
					if(!connect(client.data(), &DiscoveryClient::nodeDiscovered, [=](QString partID) {
								Q_UNUSED(partID);
								//qDebug()<<"PAIRING WIZARD partID: "<<partID;
								ui->listViewNodes->update();
				}
								)) {
						qWarning()<<"ERROR: Could not connect "<<client->objectName();
					}
				} else {
					qWarning()<<"ERROR: discovery was null";
					return;
				}
				
				replaceText(type);
				
				if(!connect(ui->widgetNetworkSettings, &NetworkSettingsWidget::addressChanged, this, &NetworkPairingActivity::onNetworkSettingsChange, OC_CONTYPE)) {
					qWarning()<<"ERROR: Could not connect "<<ui->widgetNetworkSettings->objectName();
				}
				
				
				const auto nadr = lal->currentNetworkAddress();
				
				ui->widgetNetworkSettings->setHostAddress(nadr.ip(), nadr.port());
				
			} else {
				qWarning()<<"ERROR: No local ass";
			}
			
		} else {
			qWarning()<<"ERROR: No node";
		}
	}
}


void NetworkPairingActivity::onNetworkSettingsChange(QHostAddress address, quint16 port, bool valid)
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


void NetworkPairingActivity::updateNetworkSettings()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mNode.isNull()) {
			ui->widgetNetworkSettings->configure(localAddressList());
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


QSharedPointer<AddressBook> NetworkPairingActivity::addressBook()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(nullptr!=mNode) {
			return mNode->addressBook();
		}
	}
	return nullptr;
}


QSharedPointer<LocalAddressList> NetworkPairingActivity::localAddressList()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(nullptr!=mNode) {
			return mNode->localAddressList();
		}
	}
	return nullptr;
}


bool NetworkPairingActivity::isPaierd() const{
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


void NetworkPairingActivity::startEdit(const QString &id)
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


QSharedPointer<Node> NetworkPairingActivity::node()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		return mNode;
	}
	return nullptr;
}


void NetworkPairingActivity::showEvent(QShowEvent *)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		updateNetworkSettings();
	}
}


void NetworkPairingActivity::hideEvent(QHideEvent *)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		updateNetworkSettings();
	}
}


void NetworkPairingActivity::complete()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(isPaierd()){
			pop();
		}
		else{
			switch(mNode->nodeRole()){
			case ROLE_AGENT:
				push("MessageActivity", QStringList() << "encouragement" << "" << mEncouragmentTextAgent << ":/icons/warning.svg" << "true");
				break;
			case ROLE_CONTROL:
				push("MessageActivity", QStringList() << "encouragement" << "" << mEncouragmentTextControl << ":/icons/warning.svg" << "true");
				break;
			default:
			case ROLE_UNKNOWN:
				pop();
			}
		}
	}
}


void NetworkPairingActivity::cameraPair()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		push("CameraPairingActivity");
	}
}

void NetworkPairingActivity::refresh()
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


void NetworkPairingActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
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


void NetworkPairingActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(arguments.size() == 1){
			auto id = arguments[0];
		}
	}
}


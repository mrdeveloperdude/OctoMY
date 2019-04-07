#include "PairingWizard.hpp"
#include "ui_PairingWizard.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include "app/Settings.hpp"
#include "app/Constants.hpp"

#include "utility/ui/Ui.hpp"

#include "node/Node.hpp"
#include "identity/Identicon.hpp"
#include "address/Associate.hpp"
#include "security/PortableID.hpp"

#include "address/AddressEntry.hpp"
#include "pairing/PairingListModel.hpp"
#include "PairingEditButtonDelegate.hpp"

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





PairingWizard::PairingWizard(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::PairingWizard)
	, mNode(nullptr)
	, mList(nullptr)
	, mDelegate (nullptr)
	, mTrustIndex(-1)

{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->widgetMyCertificate->configure(false,true);
	mTemplate=ui->labelBodyPair->text();
	ui->labelBodyPair->setText("<h1>Please wait...<h1>");

	reset();

	// Hook onward buttons to go to the correct page in stack
	QList<QPushButton *> onwardButtons = ui->stackedWidget->findChildren<QPushButton *>(QRegularExpression("pushButtonOnward.*"));
	//qDebug()<<"FOUND "<<onwardButtons.size()<<" ONWARDs";
	for (QList<QPushButton*>::iterator it = onwardButtons.begin(), e=onwardButtons.end(); it != e; ++it) {
		QPushButton*onward=(*it);
		//qDebug()<<" + ONWARD: "<<onward->objectName();
		connect(onward, &QPushButton::clicked,this,[=](bool b) {
			Q_UNUSED(b);
			// Skip pages that are not relevant to the selection made in "basic" page
			int next = (ui->stackedWidget->currentIndex() + 1) % ui->stackedWidget->count();
			ui->stackedWidget->setCurrentIndex(next);
		}, OC_CONTYPE);
	}


	if(!QObject::connect(ui->buttonGroupTrust, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(onTrustButtonClicked(QAbstractButton *)),OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect ";
	}

	mPulsatingTrustTimer.setTimerType(Qt::PreciseTimer);
	mPulsatingTrustTimer.setInterval(1000/30); // Somewhat conservative FPS
	mPulsatingTrustTimer.setSingleShot(false);

	if(!QObject::connect(&mPulsatingTrustTimer, SIGNAL(timeout()), this, SLOT(onPulsatingTrustTimer()),OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect ";
	}

}



PairingWizard::~PairingWizard()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void PairingWizard::onNetworkSettingsChange(QHostAddress address, quint16 port, bool valid)
{
	OC_METHODGATE();
	if(valid) {
		QSharedPointer<LocalAddressList> lal=localAddressList();
		if(!lal.isNull()) {
			lal->setCurrent(address, port);
		}
	}
	updateNetworkSettings();
}

void PairingWizard::onTrustButtonClicked(QAbstractButton *button)
{
	OC_METHODGATE();
	if (ui->radioButtonTrustUnset == button) {
		mTrustIndex=0;
		ui->widgetTrustLevel->setSvgURL(":/icons/ignore.svg");
	}
	if(ui->radioButtonTrustSet == button) {
		mTrustIndex=1;
		ui->widgetTrustLevel->setSvgURL(":/icons/trust.svg");
	}
	if(ui->radioButtonTrustBlock == button) {
		mTrustIndex=2;
		ui->widgetTrustLevel->setSvgURL(":/icons/block.svg");
	}
	//mTrustIndex=utility::getSelectedButtonIndex(ui->buttonGroupTrust, -1);
	updatePulsating();
	ui->widgetTrustLevel->update();
	qDebug()<<"TRUST CHANGED TO index "<<mTrustIndex;

}


void PairingWizard::onPulsatingTrustTimer()
{
	OC_METHODGATE();
	static const QRgb palette[]= {
		0x000000
		,0xffffff
		,0x883932
		,0x67b6bd
		,0x8b5429
		,0x574200
		,0xb86962
		,0x505050
		,0x8b3f96
		,0x55a049
		,0x40318d
		,0xbfce72
		,0x787878
		,0x94e089
		,0x7869c4
		,0x9f9f9f
	};
	static const auto paletteSz = static_cast<int>(sizeof(palette) / sizeof(QRgb));
	const auto lastColor=ui->widgetTrustLevel->silhouetteForeground();
	QColor color=lastColor;
	size_t index=0;
	while(lastColor == color) {
		index=static_cast<size_t>(qrand()%paletteSz);
		color=QColor(palette[index]);
	}
	//qDebug()<<"INDEX: "<<index<<", COLOR: "<<color;
	ui->widgetTrustLevel->setSilhouetteForeground(color);
	ui->widgetTrustLevel->setSilhouetteEnabled(true);
	ui->widgetTrustLevel->update();
}



void PairingWizard::updateNetworkSettings()
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		ui->widgetNetworkSettings->configure(localAddressList());
		QSharedPointer<DiscoveryClient> client=mNode->discoveryClient();
		if(!client.isNull()) {
			const bool visible=this->isVisible();
			//TODO: Only attemt to start discovery client when address is valid
			//const bool valid=true;//mNode->localAddresses().currentAddress().isValid()
			client->activate(visible);
			ui->progressBarSearching->setVisible(visible);
		}
	}
}


void PairingWizard::updatePulsating()
{
	OC_METHODGATE();
	const bool pulsating=(isVisible() && ui->buttonGroupTrust->checkedButton() == ui->radioButtonTrustSet);
	//qDebug()<<"PULSATING="<<pulsating;
	ui->widgetTrustLevel->setSilhouetteEnabled(pulsating);
	if(pulsating) {
		mPulsatingTrustTimer.start();
	} else {
		mPulsatingTrustTimer.stop();
	}
}



void PairingWizard::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	mNode=n;
	QSharedPointer<LocalAddressList> lal=localAddressList();
	if(!mNode.isNull() && !lal.isNull()) {
		NodeType type=mNode->nodeType();
		QSharedPointer<Associate>  ass=mNode->nodeIdentity();
		if(!ass.isNull()) {
			PortableID pid=ass->toPortableID();
			//qDebug()<<"CONFIGURE PAIRING WIZ FOR "<<pid.toPortableString();
			ui->widgetMyCertificate->setPortableID(pid);
			if(nullptr==ui->listViewNodes->model()) {

				mList=OC_NEW PairingListModel(addressBook(), type, *this);
				ui->listViewNodes->setModel(mList);

				if(nullptr==mDelegate) {
					mDelegate=OC_NEW PairingEditButtonDelegate(*this);
				}
				ui->listViewNodes->setItemDelegate(mDelegate);
			}
			QSharedPointer<DiscoveryClient> client=mNode->discoveryClient();
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


			switch(type) {
			case(TYPE_ZOO):
//			default:
			case(TYPE_UNKNOWN):
			case(TYPE_AGENT): {
				ui->labelBodyPair->setText(mTemplate.replace(QRegularExpression("\\[SOURCE\\]"), "Agent").replace(QRegularExpression("\\[DEST\\]"), "Control"));
				ui->stackedWidgetNoMessage->setCurrentWidget(ui->pageNoMessageAgent);
			}
			break;
			case(TYPE_REMOTE): {
				ui->labelBodyPair->setText(mTemplate.replace(QRegularExpression("\\[SOURCE\\]"), "Remote").replace(QRegularExpression("\\[DEST\\]"), "Agent"));
				ui->stackedWidgetNoMessage->setCurrentWidget(ui->pageNoMessageControl);
			}
			break;
			case(TYPE_HUB): {
				ui->labelBodyPair->setText(mTemplate.replace(QRegularExpression("\\[SOURCE\\]"), "Hub").replace(QRegularExpression("\\[DEST\\]"), "Agent"));
				ui->stackedWidgetNoMessage->setCurrentWidget(ui->pageNoMessageControl);
			}
			break;
			}

			if(!connect(ui->widgetNetworkSettings, &NetworkSettingsWidget::addressChanged, this, &PairingWizard::onNetworkSettingsChange, OC_CONTYPE)) {
				qWarning()<<"ERROR: Could not connect "<<ui->widgetNetworkSettings->objectName();
			}



			const auto nadr=lal->currentNetworkAddress();

			ui->widgetNetworkSettings->setHostAddress(nadr.ip(), nadr.port());

		} else {
			qWarning()<<"ERROR: No local ass";
		}

	} else {
		qWarning()<<"ERROR: No node";
	}
}



QSharedPointer<AddressBook> PairingWizard::addressBook()
{
	OC_METHODGATE();
	if(nullptr!=mNode) {
		return mNode->addressBook();
	}
	return nullptr;
}

QSharedPointer<LocalAddressList> PairingWizard::localAddressList()
{
	OC_METHODGATE();
	if(nullptr!=mNode) {
		return mNode->localAddressList();
	}
	return nullptr;
}



void PairingWizard::setCurrentPage(QWidget *cur)
{
	OC_METHODGATE();
	ui->stackedWidget->setCurrentWidget(cur);
}

void PairingWizard::reset()
{
	OC_METHODGATE();
	setCurrentPage(ui->pagePairWithPeers);
}


void PairingWizard::startEdit(int row)
{
	OC_METHODGATE();
	qDebug()<<"STARTING EDIT FOR "<<row;
	QModelIndex index=mList->index(row, 0);

	if(index.isValid()) {
		//setUpdatesEnabled(false);
		QVariantMap map=index.data(Qt::DisplayRole).toMap();
		qDebug()<<"DATA FOR "<<row<<" DURING EDIT IS: "<<map;
		QSharedPointer<AddressBook> peerStore=addressBook();
		if(!mNode.isNull() && !peerStore.isNull() ) {
			mCurrentlyEditingID=map["key"].toMap()["id"].toString();
			qDebug()<<"CURRENTLY EDITING ID "<<mCurrentlyEditingID;
			QSharedPointer<Associate> peer=peerStore->associateByID(mCurrentlyEditingID);
			if(nullptr!=peer) {
				const QStringList trusts=peer->trusts();
				const NodeType type=peer->type();
				const bool take=trusts.contains("take-control");
				const bool give=trusts.contains("give-control");
				const bool block=trusts.contains("block");
				int index=0;
				if(block) {
					index=2;
				} else {
					switch(type) {
//					default:
					case(TYPE_ZOO):
					case(TYPE_UNKNOWN):
					case(TYPE_AGENT): {
						if(give) {
							index=1;
						}
					}
					break;
					case(TYPE_REMOTE): {
						if(take) {
							index=1;
						}
					}
					break;
					case(TYPE_HUB): {
						if(give || take) {
							index=1;
						}
					}
					break;
					}
				}
				mTrustIndex=index;
				utility::ui::setSelectedButtonIndex(ui->buttonGroupTrust, mTrustIndex);
				onTrustButtonClicked(ui->buttonGroupTrust->checkedButton());
				auto addresses=peer->addressList();
				QStringList list;
				for(auto address:addresses) {
					list << address->description+"="+address->address.toString();
				}
				ui->labelStats->setText("Addresses for '"+peer->identifier()+"': [ "+list.join(", ")+" ]");

				qDebug()<<"EDITING STARTS WITH trusts: "<<peer->trusts();
				qDebug()<<"EDITING STARTS WITH name: "<<peer->name();
				ui->widgetParticipantCertificate->configure(false,true);
				ui->widgetParticipantCertificate->setPortableID(peer->toPortableID());
			}
		}
		setCurrentPage(ui->pagePeerDetail);
		//	setUpdatesEnabled(true);
	} else {
		qWarning()<<"ERROR: Index was invalid for row "<<row;
	}
}

QSharedPointer<Node> PairingWizard::node()
{
	OC_METHODGATE();
	return mNode;
}

void PairingWizard::showEvent(QShowEvent *)
{
	OC_METHODGATE();
	updateNetworkSettings();
	updatePulsating();
}

void PairingWizard::hideEvent(QHideEvent *)
{
	OC_METHODGATE();
	updateNetworkSettings();
	updatePulsating();
}

void PairingWizard::on_pushButtonMaybeOnward_clicked()
{
	OC_METHODGATE();
	if(nullptr!=mNode) {
		QSharedPointer<AddressBook> store=addressBook();
		if(!store.isNull() && store->all().size() > 0) {
			emit done();
			return;
		}
	}
	setCurrentPage(ui->pageNoPeers);
}


void PairingWizard::on_pushButtonTryAgain_clicked()
{
	OC_METHODGATE();
	setCurrentPage(ui->pagePairWithPeers);
}

void PairingWizard::on_pushButtonDone_clicked()
{
	OC_METHODGATE();
	emit done();
}


void PairingWizard::on_pushButtonCameraPair_clicked()
{
	OC_METHODGATE();
	setCurrentPage(ui->pageCameraPairing);
}


void PairingWizard::on_pushButtonSaveEdits_clicked()
{
	OC_METHODGATE();
	qDebug()<<"SAVING AFTER EDIT OF "<<mCurrentlyEditingID;
	QSharedPointer<AddressBook> peers=addressBook();
	if(nullptr!=mNode && !addressBook().isNull()) {
		QSharedPointer<Associate> peer=peers->associateByID(mCurrentlyEditingID);
		if(!peer.isNull()) {
			NodeType type=peer->type();
			peer->removeTrust("take-control");
			peer->removeTrust("give-control");
			peer->removeTrust("block");
			if(2==mTrustIndex) {
				peer->addTrust("block");
			} else if(1==mTrustIndex) {
				switch(type) {
//				default:
				case(TYPE_ZOO):
				case(TYPE_UNKNOWN):
				case(TYPE_AGENT): {
					peer->addTrust("give-control");
				}
				break;
				case(TYPE_REMOTE): {
					peer->addTrust("take-control");
				}
				break;
				case(TYPE_HUB): {
					peer->addTrust("take-control");
					peer->addTrust("give-control");
				}
				break;
				}
			}
			qDebug()<<"EDITING ENDS WITH trusts: "<<peer->trusts();
			peers->save();
		} else {
			qWarning()<<"ERROR: No peer while saving trust edits";
		}
		//ui->listViewNodes->update();
		setCurrentPage(ui->pagePairWithPeers);
	} else {
		qWarning()<<"ERROR: No node while saving trust edits";
	}
	mCurrentlyEditingID="";
}


void PairingWizard::on_pushButtonRemove_clicked()
{
	OC_METHODGATE();
	if (QMessageBox::Yes == QMessageBox::question(this, "Warning", "Are you sure you want to permanently DELETE this peer?", QMessageBox::Yes|QMessageBox::No)) {
		QSharedPointer<AddressBook> peers=addressBook();
		if(!mNode.isNull() && !peers.isNull()) {
			peers->removeAssociate(mCurrentlyEditingID);
			peers->save();
		}
		else{
			qDebug()<<"ERROR: Remove failed";
		}
		setCurrentPage(ui->pagePairWithPeers);
	}
}


void PairingWizard::on_pushButtonRefresh_clicked()
{
	OC_METHODGATE();
	if(nullptr!=mList) {
		qDebug()<<mList->status();
	} else {
		qWarning()<<"No list";
	}
}


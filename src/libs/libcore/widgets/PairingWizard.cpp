#include "PairingWizard.hpp"
#include "ui_PairingWizard.h"


#include "basic/Settings.hpp"
#include "node/Node.hpp"
#include "identity/Identicon.hpp"
#include "basic/Associate.hpp"
#include "security/PortableID.hpp"
#include "utility/Standard.hpp"
#include "utility/Utility.hpp"

#include "basic/AddressEntry.hpp"
#include "models/PairingListModel.hpp"
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

#include "zoo/ZooConstants.hpp"



PairingWizard::PairingWizard(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::PairingWizard)
	, mNode(nullptr)
	, mList(nullptr)
	, mDelegate (nullptr)
	, mTrustIndex(-1)

{
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
	delete ui;
	ui=nullptr;
}

void PairingWizard::onNetworkSettingsChange(QHostAddress address, quint16 port, bool valid)
{
	if(nullptr!=mNode) {
		if(valid) {
			mNode->localAddressList().setCurrent(address, port);
		}
	}
	updateNetworkSettings();
}

void PairingWizard::onTrustButtonClicked(QAbstractButton *button)
{
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
	static const auto paletteSz = sizeof(palette) / sizeof(QRgb);
	const auto lastColor=ui->widgetTrustLevel->silhouetteForeground();
	QColor color=lastColor;
	size_t index=0;
	while(lastColor == color) {
		index=qrand()%paletteSz;
		color=QColor(palette[index]);
	}
	//qDebug()<<"INDEX: "<<index<<", COLOR: "<<color;
	ui->widgetTrustLevel->setSilhouetteForeground(color);
	ui->widgetTrustLevel->setSilhouetteEnabled(true);
	ui->widgetTrustLevel->update();
}



void PairingWizard::updateNetworkSettings()
{
	if(nullptr!=mNode) {
		ui->widgetNetworkSettings->configure(mNode->localAddressList());
		DiscoveryClient *client=mNode->discoveryClient();
		if(nullptr!=client) {
			const bool visible=this->isVisible();
			//TODO: Only attemt to start discovery client when address is valid
			const bool valid=true;//mNode->localAddresses().currentAddress().isValid()
			if(visible) {
				if(!client->isStarted()) {
					client->start();
				}
				ui->progressBarSearching->setVisible(true);
			} else {
				if(client->isStarted()) {
					client->stop();
				}
				ui->progressBarSearching->setVisible(false);
			}
		}
	}
}


void PairingWizard::updatePulsating()
{
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
	mNode=n;
	if(!mNode.isNull()) {
		DiscoveryClient *discovery=mNode->discoveryClient();
		NodeType type=mNode->type();
		QSharedPointer<Associate>  ass=mNode->nodeIdentity();
		if(nullptr!=ass) {
			PortableID pid=ass->toPortableID();
			//qDebug()<<"CONFIGURE PAIRING WIZ FOR "<<pid.toPortableString();
			ui->widgetMyCertificate->setPortableID(pid);
			if(nullptr==ui->listViewNodes->model()) {

				mList=OC_NEW PairingListModel(mNode->addressBook(),type,*this);
				ui->listViewNodes->setModel(mList);

				if(nullptr==mDelegate) {
					mDelegate=OC_NEW PairingEditButtonDelegate(*this);
				}
				ui->listViewNodes->setItemDelegate(mDelegate);
			}
			if(!connect(discovery, &DiscoveryClient::nodeDiscovered, [=](QString partID) {
			//qDebug()<<"PAIRING WIZARD partID: "<<partID;
			ui->listViewNodes->update();
			}
					   )) {
				qWarning()<<"ERROR: Could not connect";
			}


			switch(type) {
			case(TYPE_ZOO):
			default:
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

			if(!connect(ui->widgetNetworkSettings, &NetworkSettingsWidget::addressChanged, this, &PairingWizard::updateNetworkSettings, OC_CONTYPE)) {
				qWarning()<<"ERROR: Could not connect";
			}


			const auto nadr=mNode->localAddressList().currentNetworkAddress();

			ui->widgetNetworkSettings->set(nadr.ip(), nadr.port());

		} else {
			qWarning()<<"ERROR: No local ass";
		}

	} else {
		qWarning()<<"ERROR: No node";
	}
}


void PairingWizard::reset()
{
	ui->stackedWidget->setCurrentWidget(ui->pagePairWithPeers);
}


void PairingWizard::startEdit(int row)
{
	qDebug()<<"STARTING EDIT FOR "<<row;
	QModelIndex index=mList->index(row,0);
	if(index.isValid()) {
		//setUpdatesEnabled(false);
		QVariantMap map=index.data(Qt::DisplayRole).toMap();
		qDebug()<<"DATA FOR "<<row<<" DURING EDIT IS: "<<map;
		if(nullptr!=mNode) {
			AddressBook &peerStore=mNode->addressBook();
			mCurrentlyEditing=map["key"].toMap()["id"].toString();
			qDebug()<<"CURRENTLY EDITING ID "<<mCurrentlyEditing;
			QSharedPointer<Associate> peer=peerStore.associateByID(mCurrentlyEditing);
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
					default:
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
				utility::setSelectedButtonIndex(ui->buttonGroupTrust, mTrustIndex);
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
		ui->stackedWidget->setCurrentWidget(ui->pagePeerDetail);
		//	setUpdatesEnabled(true);
	} else {
		qWarning()<<"ERROR: Index was invalid for row "<<row;
	}
}

QSharedPointer<Node> PairingWizard::node()
{
	return mNode;
}

void PairingWizard::showEvent(QShowEvent *)
{
	updateNetworkSettings();
	updatePulsating();
}

void PairingWizard::hideEvent(QHideEvent *)
{
	updateNetworkSettings();
	updatePulsating();
}

void PairingWizard::on_pushButtonMaybeOnward_clicked()
{
	if(nullptr!=mNode) {
		AddressBook &store=mNode->addressBook();
		if(store.all().size()>0) {
			emit done();
			return;
		}
	}
	ui->stackedWidget->setCurrentWidget(ui->pageNoPeers);
}


void PairingWizard::on_pushButtonTryAgain_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pagePairWithPeers);
}

void PairingWizard::on_pushButtonDone_clicked()
{
	emit done();
}


void PairingWizard::on_pushButtonCameraPair_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageCameraPairing);
}

void PairingWizard::on_pushButtonSaveEdits_clicked()
{
	qDebug()<<"SAVING AFTER EDIT OF "<<mCurrentlyEditing;
	if(nullptr!=mNode) {
		AddressBook &peers=mNode->addressBook();
		QSharedPointer<Associate> peer=peers.associateByID(mCurrentlyEditing);
		if(nullptr!=peer) {
			NodeType type=peer->type();
			peer->removeTrust("take-control");
			peer->removeTrust("give-control");
			peer->removeTrust("block");
			if(2==mTrustIndex) {
				peer->addTrust("block");
			} else if(1==mTrustIndex) {
				switch(type) {
				default:
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
			peers.save();
		} else {
			qWarning()<<"ERROR: No peer while saving trust edits";
		}
		//ui->listViewNodes->update();
		ui->stackedWidget->setCurrentWidget(ui->pagePairWithPeers);
	} else {
		qWarning()<<"ERROR: No node while saving trust edits";
	}
	mCurrentlyEditing="";
}

void PairingWizard::on_pushButtonRemove_clicked()
{
	if (QMessageBox::Yes == QMessageBox::question(this, "Warning", "Are you sure you want to permanently DELETE this peer?", QMessageBox::Yes|QMessageBox::No)) {

		if(nullptr!=mNode) {
			AddressBook &peers=mNode->addressBook();
			peers.removeAssociate(mCurrentlyEditing);
			peers.save();
		}
		ui->stackedWidget->setCurrentWidget(ui->pagePairWithPeers);
	}
}

void PairingWizard::on_pushButtonRefresh_clicked()
{
	if(nullptr!=mList) {
		qDebug()<<mList->status();
	} else {
		qWarning()<<"No list";
	}
}


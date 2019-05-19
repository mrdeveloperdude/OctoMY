#include "PairingTrustWidget.hpp"
#include "ui_PairingTrustWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

PairingTrustWidget::PairingTrustWidget(QWidget *parent) :
	QWidget(parent),
	ui(OC_NEW Ui::PairingTrustWidget)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


PairingTrustWidget::~PairingTrustWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void PairingWizard::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mPulsatingTrustTimer.setTimerType(Qt::PreciseTimer);
		// Somewhat conservative FPS
		mPulsatingTrustTimer.setInterval(1000/30);
		mPulsatingTrustTimer.setSingleShot(false);

		if(!QObject::connect(&mPulsatingTrustTimer, SIGNAL(timeout()), this, SLOT(onPulsatingTrustTimer()),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect ";
		}
		if(!QObject::connect(ui->buttonGroupTrust, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(onTrustButtonClicked(QAbstractButton *)), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect ";
		}



		ui->widgetParticipantCertificate->configure(false, true);
		ui->widgetParticipantCertificate->setPortableID(peer->toPortableID());

		auto addresses=peer->addressList();
		QStringList list;
		for(auto address:addresses) {
			list << address->description+"="+address->address.toString();
		}
		ui->labelStats->setText("Addresses for '"+peer->identifier()+"': [ "+list.join(", ")+" ]");


	}
}


void PairingTrustWidget::updatePulsating()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		const bool pulsating=(isVisible() && ui->buttonGroupTrust->checkedButton() == ui->radioButtonTrustSet);
		//qDebug()<<"PULSATING="<<pulsating;
		ui->widgetTrustLevel->setSilhouetteEnabled(pulsating);
		if(pulsating) {
			mPulsatingTrustTimer.start();
		} else {
			mPulsatingTrustTimer.stop();
		}
	}
}


void PairingTrustWidget::startEdit(int row)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
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

					qDebug()<<"EDITING STARTS WITH trusts: "<<peer->trusts();
					qDebug()<<"EDITING STARTS WITH name: "<<peer->name();

				}
			}
			setCurrentPage(ui->pagePeerDetail);
			//	setUpdatesEnabled(true);
		} else {
			qWarning()<<"ERROR: Index was invalid for row "<<row;
		}
	}
}


void PairingTrustWidget::showEvent(QShowEvent *)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		updatePulsating();
	}
}


void PairingTrustWidget::hideEvent(QHideEvent *)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		updatePulsating();
	}
}


void PairingTrustWidget::onTrustButtonClicked(QAbstractButton *button)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
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
}


void PairingTrustWidget::onPulsatingTrustTimer()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
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
}



void PairingTrustWidget::on_pushButtonSaveEdits_clicked()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
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
}


void PairingTrustWidget::on_pushButtonRemove_clicked()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if (QMessageBox::Yes == QMessageBox::question(this, "Warning", "Are you sure you want to permanently DELETE this peer?", QMessageBox::Yes|QMessageBox::No)) {
			QSharedPointer<AddressBook> peers=addressBook();
			if(!mNode.isNull() && !peers.isNull()) {
				peers->removeAssociate(mCurrentlyEditingID);
				peers->save();
			} else {
				qDebug()<<"ERROR: Remove failed";
			}
			setCurrentPage(ui->pagePairWithPeers);
		}
	}
}

#include "PairingTrustWidget.hpp"
#include "ui_PairingTrustWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include "utility/ui/Ui.hpp"

#include "address/Associate.hpp"
#include "address/AddressList.hpp"
#include "address/AddressEntry.hpp"

#include "node/Node.hpp"

#include "address/TrustList.hpp"

#include <QMessageBox>


PairingTrustWidget::PairingTrustWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::PairingTrustWidget)
	, mConfigureHelper("PairingTrustWidget", true, false, true, true, false)
{
	OC_METHODGATE();
	qRegisterMetaType<TrustLevel>("TrustLevel");
	ui->setupUi(this);
}


PairingTrustWidget::~PairingTrustWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


void PairingTrustWidget::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode=n;
		ui->widgetTrustLevel->configure(true);
		ui->widgetParticipantCertificate->configure(false, true);
		mPulsatingTrustTimer.setTimerType(Qt::PreciseTimer);

		// Somewhat conservative FPS
		mPulsatingTrustTimer.setInterval(1000/30);
		mPulsatingTrustTimer.setSingleShot(false);
		if(!QObject::connect(&mPulsatingTrustTimer, SIGNAL(timeout()), this, SLOT(onPulsatingTrustTimer()), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect ";
		}
		if(!QObject::connect(ui->buttonGroupTrust, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(onTrustButtonClicked(QAbstractButton *)), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect ";
		}
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


void PairingTrustWidget::startEdit(QSharedPointer<Associate> peer)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		qDebug()<<"STARTING EDIT FOR "<<peer->toPortableID();
		if(!peer.isNull()) {
			ui->widgetParticipantCertificate->setPortableID(peer->toPortableID());
			auto addresses=peer->addressList();
			QStringList list;
			for(auto address:addresses) {
				list << address->description+"="+address->address.toString();
			}
			ui->labelStats->setText("Addresses for '"+peer->identifier()+"': [ "+list.join(", ")+" ]");
			TrustLevel level=peer->trusts().toTrustLevel(peer->type());
			qDebug()<<"EDITING STARTS WITH id: "<<peer->id();
			//onTrustButtonClicked(ui->buttonGroupTrust->checkedButton());
			selectTrustLevel(level);
			mCurrentID=peer->id();
			setEnabled(true);
		}
	}
}


TrustLevel PairingTrustWidget::selectedTrustLevel() const
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		TrustLevel level;
		const QAbstractButton *button=ui->buttonGroupTrust->checkedButton();
		if (ui->radioButtonTrustUnset == button) {
			level=IGNORE;
		} else if(ui->radioButtonTrustSet == button) {
			level=TRUST;
		} else if(ui->radioButtonTrustBlock == button) {
			level=BLOCK;
		} else {
			level=IGNORE;
		}
		return level;
	}
	return IGNORE;
}


void PairingTrustWidget::selectTrustLevel(TrustLevel level)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		int index=0;
		switch(level) {
		case(TrustLevel::BLOCK): {
			index=2;
		}
		break;
		case(TrustLevel::TRUST): {
			index=1;
		}
		break;
		case(TrustLevel::IGNORE): {
			index=0;
		}
		break;
		}
		utility::ui::setSelectedButtonIndex(ui->buttonGroupTrust, index);
		ui->widgetTrustLevel->setTrustLevel(level);
		updatePulsating();
		ui->widgetTrustLevel->update();
		qDebug()<<"TRUST CHANGED TO "<<trustLevelToString(level);
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
	Q_UNUSED(button);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		selectTrustLevel(selectedTrustLevel());
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
		/*
		QString mCurrentlyEditingID;
		qDebug()<<"SAVING AFTER EDIT OF "<<mCurrentlyEditingID;
		QSharedPointer<AddressBook> peers=addressBook();
		auto addressBook=mNode->addressBook();
		if(!addressBook.isNull()) {
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
		*/
		qDebug()<<"EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEDIT COMPLETE";
		setEnabled(false);
		emit editComplete(mCurrentID, selectedTrustLevel(), true);
		mCurrentID="";
	}
}


void PairingTrustWidget::on_pushButtonRemove_clicked()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if (QMessageBox::Yes == QMessageBox::question(this, "Warning", "Are you sure you want to permanently DELETE this peer?", QMessageBox::Yes|QMessageBox::No)) {
			setEnabled(false);
			emit remove(mCurrentID);
			mCurrentID="";
		}
	}
}

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
		ui->widgetTrustLevel->configure();
		ui->widgetParticipantCertificate->configure(false, true);

		if(!QObject::connect(ui->buttonGroupTrust, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(onTrustButtonClicked(QAbstractButton *)), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect ";
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
		qDebug()<<"TRUST CHANGED TO "<<trustLevelToString(level);
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


void PairingTrustWidget::on_pushButtonSaveEdits_clicked()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
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

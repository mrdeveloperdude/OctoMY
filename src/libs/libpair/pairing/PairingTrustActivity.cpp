#include "PairingTrustActivity.hpp"
#include "ui_PairingTrustActivity.h"

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


PairingTrustActivity::PairingTrustActivity(QWidget *parent)
	: Activity(parent)
	, ui(OC_NEW Ui::PairingTrustActivity)
	, mConfigureHelper("PairingTrustActivity", true, false, true, true, false)
{
	OC_METHODGATE();
	qRegisterMetaType<TrustLevel>("TrustLevel");
	ui->setupUi(this);
}


PairingTrustActivity::~PairingTrustActivity()
{
	OC_METHODGATE();
	delete ui;
	ui = nullptr;
}


void PairingTrustActivity::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = n;
		ui->widgetTrustLevel->configure();
		ui->widgetParticipantCertificate->configure(false, true);
		if(!connect(ui->buttonGroupTrust, &QButtonGroup::buttonClicked, this, &PairingTrustActivity::onTrustButtonClicked, OC_CONTYPE)) {
			qWarning() << "ERROR: Could not connect ";
		}
	}
}


TrustLevel PairingTrustActivity::trustLevel() const
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		TrustLevel level;
		const QAbstractButton *button = ui->buttonGroupTrust->checkedButton();
		if (ui->radioButtonTrustUnset == button) {
			level = IGNORE;
		} else if(ui->radioButtonTrustSet == button) {
			level = TRUST;
		} else if(ui->radioButtonTrustBlock == button) {
			level = BLOCK;
		} else {
			level = IGNORE;
		}
		return level;
	}
	return IGNORE;
}


void PairingTrustActivity::setTrustLevel(TrustLevel level)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		int index=0;
		switch(level) {
			case(TrustLevel::BLOCK): {
				index = 2;
			}
			break;
			case(TrustLevel::TRUST): {
				index = 1;
			}
			break;
			default:
			case(TrustLevel::IGNORE): {
				index = 0;
			}
			break;
		}
		utility::ui::setSelectedButtonIndex(ui->buttonGroupTrust, index);
		ui->widgetTrustLevel->setTrustLevel(level);
		qDebug() << "TRUST CHANGED TO " << trustLevelToString(level);
	}
}


void PairingTrustActivity::onTrustButtonClicked(QAbstractButton *button)
{
	OC_METHODGATE();
	Q_UNUSED(button);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		setTrustLevel(trustLevel());
	}
}


void PairingTrustActivity::finish(TrustLevel level){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		qDebug() << "FINISHING WITH TRUST" << trustLevelToString(level);
		bool confirmed{false};
		if(REMOVE == level){
			confirmed = (QMessageBox::Yes == QMessageBox::question(this, "Warning", "Are you sure you want to permanently DELETE this peer?", QMessageBox::Yes|QMessageBox::No));
		}
		else{
			confirmed = true;
		}
		if(confirmed){
			setEnabled(false);
			pop(QStringList() << trustLevelToString(level));
		}
	}
}


void PairingTrustActivity::saveTrust()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		finish(trustLevel());
	}
}


void PairingTrustActivity::removeTrust()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		finish(REMOVE);
	}
}


void PairingTrustActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}


void PairingTrustActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		bool startOK{false};
		if(arguments.size() == 1){
			auto id = arguments[0];
			auto peer = mNode->addressBook()->associateByID(id);
			if(peer){
				qDebug() << "STARTING EDIT FOR " << peer->toPortableID();
				if(!peer.isNull()) {
					ui->widgetParticipantCertificate->setPortableID(peer->toPortableID());
					auto addresses = peer->addressList();
					QStringList list;
					for(auto &address:addresses) {
						list << address->description + "=" + (address->address.isNull()?"NULL":address->address->toString());
					}
					ui->labelStats->setText("Addresses for '" + peer->identifier() + "': [ " + list.join(", ") + " ]");
					auto level = peer->trusts().toTrustLevel(peer->type());
					qDebug() << "EDITING STARTS WITH id: " << peer->id() << "on level" << level;
					setTrustLevel(level);
					startOK = true;
				}
			}
			else{
				qWarning() << "No peer for '" << id << "'";
			}
		}
		else{
			qWarning() << "Wrong number of arguments";
		}
		if(startOK){
			setEnabled(true);
		}
		else{
			finish(IGNORE);
		}
	}
}


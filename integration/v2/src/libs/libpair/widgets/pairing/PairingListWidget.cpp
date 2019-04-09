#include "PairingListWidget.hpp"
#include "ui_PairingListWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include "pairing/PairingListModel.hpp"
#include "PairingEditButtonDelegate.hpp"

#include "discovery/DiscoveryClient.hpp"

#include "node/Node.hpp"
#include "address/AddressList.hpp"


PairingListWidget::PairingListWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::PairingListWidget)
	, mNode(nullptr)
	, mList(nullptr)
	, mDelegate (nullptr)
	, mConfigureHelper("PairingListWidget", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

PairingListWidget::~PairingListWidget()
{
	OC_METHODGATE();
	delete ui;
}


void PairingListWidget::configure(QSharedPointer<Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode=node;
		QSharedPointer<LocalAddressList> lal= mNode->localAddressList();
		if(!mNode.isNull() && !lal.isNull()) {
			NodeType type=mNode->nodeType();
			if(nullptr==ui->listViewNodes->model()) {
				mList=OC_NEW PairingListModel(mNode->addressBook(), type);
				ui->listViewNodes->setModel(mList);
				if(nullptr==mDelegate) {
					mDelegate=OC_NEW PairingEditButtonDelegate(this);
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
		}
	}
}

#include "DevelopmentMenuActivity.hpp"

#include "address/Associate.hpp"
#include "hub/HubWindow.hpp"
#include "node/Node.hpp"
#include "node/NodeFactory.hpp"
#include "uptime/MethodGate.hpp"

#include <QDebug>

DevelopmentMenuActivity::DevelopmentMenuActivity(QWidget *parent)
	: MenuActivity(parent, "DevelopmentMenuActivity")
	, mConfigureHelper("DevelopmentMenuActivity", true, false, false, true, false)
	
{
	OC_METHODGATE();
}


DevelopmentMenuActivity::~DevelopmentMenuActivity()
{
	OC_METHODGATE();
}


void DevelopmentMenuActivity::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		auto hubWindow = qSharedPointerCast<HubWindow>(n->nodeWindow());
		//auto hw = hubWindow.data();
		////////////////////////////////////////////////////////////////////////
		addGroup(tr("Development"));
		
		addButton(tr("Constellation"), ":/icons/plan/flock.svg",             tr("Start a constellation of nodes for testing"), [](){
			NodeFactory nodeFactory("development-");
			const auto agentStatus = nodeFactory.forkNodeEventloop(NodeType::TYPE_AGENT);
			const auto remoteStatus = nodeFactory.forkNodeEventloop(NodeType::TYPE_REMOTE);
			qDebug() << "CONSTILLATIONS" << agentStatus << remoteStatus;
		});

		addButton(tr("Resources"),          ":/icons/app/folder.svg",              tr("Work with internal (Qt) resources"),                "InternalResourcesActivity");
		addButton(tr("Network"),            ":/icons/pairing/lan.svg",             tr("Network utilities"),                                "NetworkUtilsActivity");
		addButton(tr("Illustration"),       ":/icons/general/illustration.svg",    tr("Work with illustrating widgets for documentation"), "WidgetIllustrationActivity");
		addButton(tr("Key Management"),     ":/icons/identity/key.svg",            tr("Manage Cryptographic Keys"),                        "PKIManagerActivity");
		addButton(tr("QR Utility"),         ":/icons/pairing/qrcode.svg",          tr("Work with QR codes"),                               "QRUtilityActivity");
		addButton(tr("Camera"),             ":/icons/pairing/snap.svg",            tr("Work with camera"),                                 "CameraActivity");
		addButton(tr("Compute"),            ":/icons/general/compute.svg",         tr("Work with compute"),                                "ComputeActivity");
		addButton(tr("Serial"),             ":/icons/pairing/serial.svg",          tr("Work with serial communications"),                  "SerialActivity");


		addSpacer();
	}
}

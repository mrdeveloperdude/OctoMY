#include "HubMainMenu.hpp"

#include "address/Associate.hpp"
#include "hub/HubWindow.hpp"
#include "node/Node.hpp"
#include "node/NodeFactory.hpp"
#include "uptime/MethodGate.hpp"

#include <QDebug>

HubMainMenu::HubMainMenu(QWidget *parent)
	: MenuActivity(parent, "HubMainMenu")
	, mConfigureHelper("HubMainMenu", true, false, false, true, false)
	
{
	OC_METHODGATE();
}


HubMainMenu::~HubMainMenu()
{
	OC_METHODGATE();
}


void HubMainMenu::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		auto hubWindow = qSharedPointerCast<HubWindow>(n->nodeWindow());
		auto hw = hubWindow.data();
		bool devMode{true};
		////////////////////////////////////////////////////////////////////////
		addGroup(tr("State"));
		
		addButton(tr("Toggle Online"),      ":/icons/pairing/connected.svg",   tr("Toggle availability of Agent on networks"),  &HubWindow::toggleOnline, hw, true, false);
		
		////////////////////////////////////////////////////////////////////////
		addGroup(tr("Setup"));
		
		if(devMode)
			addButton(tr("Unboxing"),       ":/icons/app/unboxing.svg",        tr("Set up this Remote from scratch"),           &HubWindow::unboxing, hw);
		addButton(tr("Identity"),           ":/icons/pairing/identity.svg",    tr("Manage the identity of this Remote"),        &HubWindow::identity, hw);
		addButton(tr("Pairing"),            ":/icons/pairing/pair.svg",        tr("Manage the pairing of this Remote"),         &HubWindow::pairing, hw);
		addButton(tr("Connection"),         ":/icons/controller/network.svg",  tr("Manage Remote connection"),                  &HubWindow::connection, hw);
		
		////////////////////////////////////////////////////////////////////////
		addGroup(tr("Utilities"));
		
		addButton(tr("Development"),        ":/icons/debug/development.svg",   tr("Development tools"),                         "DevelopmentMenuActivity");
		addButton(tr("Utilities"),          ":/icons/general/tools.svg",       tr("Useful utilities"),                          "UtilitiesMenuActivity");
		
		////////////////////////////////////////////////////////////////////////
		addSpacer();
		addGroup(tr(""));
		
		addButton(tr("Exit"),          ":/icons/general/on.svg",               tr("Terminate this program"),                    &HubWindow::requestApplicationShutdown, hw);
	}
}

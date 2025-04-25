#include "RemoteMainMenu.hpp"

#include "address/Associate.hpp"
#include "remote/RemoteWindow.hpp"
#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"

#include <QDebug>

RemoteMainMenu::RemoteMainMenu(QWidget *parent)
	: MenuActivity(parent, "RemoteMainMenu")
	, mConfigureHelper("RemoteMainMenu", true, false, false, true, false)

{
	OC_METHODGATE();
}


RemoteMainMenu::~RemoteMainMenu()
{
	OC_METHODGATE();
}


void RemoteMainMenu::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		MenuActivity::configure();
		auto remoteWindow = qSharedPointerCast<RemoteWindow>(n->nodeWindow());
		auto hw = remoteWindow.data();
		bool devMode{true};
		addButton(tr("Toggle Online"),      ":/icons/pairing/connected.svg",   tr("Toggle availability of Remote on networks"),  &RemoteWindow::toggleOnline, hw, true, false);
		
		if(devMode)
			addButton(tr("Unboxing"),       ":/icons/app/unboxing.svg",        tr("Set up this Remote from scratch"),           &RemoteWindow::unboxing, hw);
		addButton(tr("Identity"),           ":/icons/pairing/identity.svg",    tr("Manage the identity of this Remote"),        &RemoteWindow::identity, hw);
		addButton(tr("Pairing"),            ":/icons/pairing/pair.svg",        tr("Manage the pairing of this Remote"),         &RemoteWindow::pairing, hw);
		addButton(tr("Connection"),         ":/icons/controller/network.svg",  tr("Manage Remote connection"),                  &RemoteWindow::connection, hw);
		addSpacer();
		addButton(tr("Exit"),               ":/icons/general/on.svg",          tr("Terminate this program"),                    &RemoteWindow::requestApplicationShutdown, hw);
	}

}

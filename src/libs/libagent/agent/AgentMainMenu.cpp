#include "AgentMainMenu.hpp"

#include "address/Associate.hpp"
#include "agent/AgentWindow.hpp"
#include "node/Node.hpp"
#include "node/NodeWindow.hpp"
#include "uptime/MethodGate.hpp"

#include <QDebug>

AgentMainMenu::AgentMainMenu(QWidget *parent)
	: MenuActivity(parent, "AgentMainMenu")
	, mConfigureHelper("AgentMainMenu", true, false, false, true, false)

{
	OC_METHODGATE();
}


AgentMainMenu::~AgentMainMenu()
{
	OC_METHODGATE();
}


void AgentMainMenu::configure(QSharedPointer<Node> n)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		MenuActivity::configure();
		mNode = n;
		auto agentWindow = qSharedPointerCast<AgentWindow>(n->nodeWindow());
		auto hw = agentWindow.data();
		bool devMode{true};
		addButton(tr("Toggle Online"),      ":/icons/pairing/connected.svg",   tr("Toggle availability of Agent on networks"),  &AgentWindow::toggleOnline, hw, true, false);
		
		if(devMode)
			addButton(tr("Unboxing"),       ":/icons/app/unboxing.svg",        tr("Set up this Remote from scratch"),           &AgentWindow::unboxing, hw);
		addButton(tr("Identity"),           ":/icons/pairing/identity.svg",    tr("Manage the identity of this Remote"),        &AgentWindow::identity, hw);
		addButton(tr("Pairing"),            ":/icons/pairing/pair.svg",        tr("Manage the pairing of this Remote"),         &AgentWindow::pairing, hw);
		addButton(tr("Connection"),         ":/icons/controller/network.svg",  tr("Manage Remote connection"),                  &AgentWindow::connection, hw);
		addSpacer();
		addButton(tr("Exit"),               ":/icons/general/on.svg",          tr("Terminate this program"),                    &AgentWindow::requestApplicationShutdown, hw);
	}
	
	
	/* Other stuff
	
		mShowOnlineButtonAction->setStatusTip(tr("Show connect button in main screen"));
		mShowOnlineButtonAction->setCheckable(true);
		connect(mShowOnlineButtonAction, &QAction::triggered, this, &AgentWindow::onOnlineButtonVisibilityChanged);
		mMenu.addAction(mShowOnlineButtonAction);
	// Show Face
	//////////////////
	
	
	// Unbirth
	//////////////////
	mUnbirthAction->setStatusTip(tr("Delete the identity of this agent to restart birth"));
	mUnbirthAction->setIcon(QIcon(":/icons/kill.svg"));
	connect(mUnbirthAction, &QAction::triggered, this, &AgentWindow::onStartUnbirth);
	mMenu.addAction(mUnbirthAction);
	*/
	
}




void AgentMainMenu::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	//qDebug() << "PUSH HUB MAIN MENU ACTIVITY: " << arguments;
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}


void AgentMainMenu::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if("MessageActivity" == returnActivity){
			if(returnArguments.count()>0 && returnArguments[0] == "quit"){
				auto agentWindow = qSharedPointerCast<AgentWindow>(mNode->nodeWindow());
				agentWindow->applicationShutdown();
			}
		}
	}
}

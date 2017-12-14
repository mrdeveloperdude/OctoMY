#include "Hub.hpp"
#include "HubWindow.hpp"

#include "node/AppContext.hpp"



Hub::Hub(NodeLauncher<Hub> &launcher, QObject *parent)
	: Node(launcher, OC_NEW AppContext(launcher.options(), launcher.environment(), "hub", parent), ROLE_CONTROL, TYPE_HUB, parent)
{
}

Hub::~Hub()
{
}



QSharedPointer<QWidget> Hub::showWindow()
{
	if(mWindow.isNull()) {
		mWindow=QSharedPointer<HubWindow>(OC_NEW HubWindow(QEnableSharedFromThis<Hub>::sharedFromThis(), nullptr));
	}
	if(!mWindow.isNull()) {
		mWindow->show();
	}
	return mWindow;
}



QSharedPointer<Node> Hub::sharedThis()
{
	OC_METHODGATE();
	return sharedFromThis();
}

#include "Hub.hpp"
#include "HubWindow.hpp"

#include "node/AppContext.hpp"



Hub::Hub(NodeLauncher<Hub> &launcher, QObject *parent)
	: Node(OC_NEW AppContext(launcher.options(), launcher.environment(), "hub", parent), ROLE_CONTROL, TYPE_HUB, parent)
	, mWindow(nullptr)
	, mThis(this)
{
}

Hub::~Hub()
{
}



QWidget *Hub::showWindow()
{
	if(nullptr==mWindow) {
		mWindow=OC_NEW HubWindow(mThis, nullptr);
	}
	if(nullptr!=mWindow) {
		mWindow->show();
	}
	return mWindow;
}


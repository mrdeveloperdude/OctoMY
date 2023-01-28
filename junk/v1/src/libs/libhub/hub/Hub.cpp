#include "Hub.hpp"
#include "HubWindow.hpp"

#include "node/AppContext.hpp"



Hub::Hub(NodeLauncher<Hub> &launcher, QObject *parent)
	: Node(launcher, OC_NEW AppContext(launcher.options(), launcher.environment(), "hub", parent), ROLE_CONTROL, TYPE_HUB, parent)
{
	OC_METHODGATE();
	// NOTE: Please do not put code here that generates events. Instead put them in init()
}

Hub::~Hub()
{
	OC_METHODGATE();
}


void Hub::init()
{
	OC_METHODGATE();
	Node::init();
	//qDebug()<<"EMITTING LOAD COMLPETE";
	emit appLoaded();
}


void Hub::deInit()
{
	OC_METHODGATE();
	Node::deInit();
}


QSharedPointer<QWidget> Hub::showWindow()
{
	OC_METHODGATE();
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

#include "Remote.hpp"

#include "utility/Standard.hpp"
#include "comms/CommsSession.hpp"
#include "comms/CommsSessionDirectory.hpp"
#include "RemoteWindow.hpp"
#include "node/AppContext.hpp"



#include <QDebug>
#include <QDataStream>
#include <QSharedPointer>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>


Remote::Remote(NodeLauncher<Remote> &launcher, QObject *parent)
	: Node(launcher, OC_NEW AppContext(launcher.options(), launcher.environment(), "remote", parent), ROLE_CONTROL, TYPE_REMOTE, parent)
{
	OC_METHODGATE();
	// NOTE: Please do not put code here that generates events. Instead put them in init()
}

Remote::~Remote()
{
	OC_METHODGATE();
}


void Remote::init()
{
	OC_METHODGATE();
	Node::init();

	qDebug()<<"EMITTING LOAD COMLPETE";
	emit appLoaded();
}


void Remote::deInit()
{
	OC_METHODGATE();
	Node::deInit();
}


QSharedPointer<QWidget> Remote::showWindow()
{
	OC_METHODGATE();
	if(mWindow.isNull()) {
		mWindow=QSharedPointer<RemoteWindow>(OC_NEW RemoteWindow(QEnableSharedFromThis<Remote>::sharedFromThis(), nullptr));
	}
	if(!mWindow.isNull()) {
		mWindow->show();
	}
	return mWindow;
}




void Remote::setNodeCouriersRegistration(bool reg)
{
	OC_METHODGATE();
	Node::setNodeCouriersRegistration(reg);
	// When we get a new remote specific courier, put it here
	/*
	if(nullptr!=mComms){
		if(nullptr!=mRemoteCourierX){
			mComms->setCourierRegistered(*mRemoteCourierX, reg);
		}

	}*/
}


QSharedPointer<Node> Remote::sharedThis()
{
	OC_METHODGATE();
	return sharedFromThis();
}

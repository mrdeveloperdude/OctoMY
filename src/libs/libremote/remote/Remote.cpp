#include "Remote.hpp"

#include "node/NodeWindow.hpp"

#include "widgets/remote/RemoteWindow.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "app/AppContext.hpp"

#include "comms/CommsSession.hpp"
#include "comms/CommsSessionDirectory.hpp"

#include <QDebug>
/*
#include <QDataStream>
#include <QSharedPointer>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>
*/

Remote::Remote()
	: mNodeConfigureHelper("Remote", true, true, false, Constants::OC_LOG_CONFIGURE_HELPER_WARNINGS, Constants::OC_LOG_CONFIGURE_HELPER_CHANGES)
	  //: Node(launcher, OC_NEW AppContext(launcher.options(), launcher.environment(), "remote", parent), ROLE_CONTROL, TYPE_REMOTE, parent)
{
	OC_METHODGATE();
	//qDebug()<<"Remote()";
	// NOTE: Please do not put code here that generates events. Instead put them in *configure*() or *activate*()
}


Remote::~Remote()
{
	OC_METHODGATE();
	//qDebug()<<"~Remote()";
}


void Remote::nodeConfigure()
{
	OC_METHODGATE();
	//qDebug()<<"nodeConfigure()";
	if(mNodeConfigureHelper.configure()) {
		auto ctx=context();
		if(!ctx.isNull()) {

		}
	} else {
		qDebug()<<"ERROR: No context, closing";
		emit nodeRequestExit(EXIT_FAILURE);
	}
}


void Remote::nodeActivate(const bool on)
{
	OC_METHODGATE();
	if(mNodeConfigureHelper.activate(on)) {
		//qDebug()<<"nodeActivate(on="<<on<<")";
		if(on) {
			emit nodeActivateChanged(on);
		} else {
			if(!mWindow.isNull()) {
				mWindow.clear();
			}
			emit nodeActivateChanged(on);
		}
	}
}


QSharedPointer<NodeWindow> Remote::nodeWindow()
{
	OC_METHODGATE();
	//qDebug()<<"nodeWindow()";
	if(mWindow.isNull()) {
		QSharedPointer<Remote> sp=qSharedPointerCast<Remote>(sharedThis());
		if(!sp.isNull()) {
			mWindow=QSharedPointer<RemoteWindow>(OC_NEW RemoteWindow(nullptr));
			if(!mWindow.isNull()) {
				mWindow->nodeWindowConfigure(sp);
			} else {
				qWarning()<<"ERROR: Could not allocate RemoteWindow";
			}
		} else {
			qWarning()<<"ERROR: Shared pointer to this was null for remote";
		}
	}
	return mWindow;
}


NodeRole Remote::nodeRole()
{
	OC_METHODGATE();
	return ROLE_CONTROL;
}


NodeType Remote::nodeType()
{
	OC_METHODGATE();
	return TYPE_REMOTE;
}


void Remote::registerNodeCouriers(bool reg)
{
	OC_METHODGATE();
	Node::registerNodeCouriers(reg);
	// When we get a new remote specific courier, put it here
	/*
	if(!mComms.isNull()){
		if(nullptr!=mRemoteCourierX){
			mComms->setCourierRegistered(*mRemoteCourierX, reg);
		}

	}*/
}


QSharedPointer<Node> Remote::sharedThis()
{
	OC_METHODGATE();
	//return QEnableSharedFromThis<Remote>::sharedFromThis();
	return sharedFromThis();
}

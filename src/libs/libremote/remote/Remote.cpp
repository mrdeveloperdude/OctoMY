#include "Remote.hpp"

#include "node/NodeWindow.hpp"
#include "remote/RemoteWindow.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "app/AppContext.hpp"

#include <QDebug>
/*
#include <QDataStream>
#include "uptime/SharedPointerWrapper.hpp"
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>
*/


/*!
	\class Remote
	\brief The Remote application
	\since 1.0
	\ingroup app
	\inmodule remote
	
	Remote represents a remote control to control one or more agents
	
	\sa RemoteMain, Agent, Hub, Zoo
*/


Remote::Remote()
	: mNodeConfigureHelper("Remote", true, true, false, Constants::OC_LOG_CONFIGURE_HELPER_WARNINGS, Constants::OC_LOG_CONFIGURE_HELPER_CHANGES)
	  //: Node(launcher, OC_NEW AppContext(launcher.options(), launcher.environment(), "remote", parent), ROLE_CONTROL, TYPE_REMOTE, parent)
{
	OC_METHODGATE();
	//qDebug()<<"Remote()";
	// NOTE: Please do not put code here that generates events. Instead put them in *nodeConfigure*() or *nodeActivate*()
}

/*! 
   Destructor
 */
Remote::~Remote()
{
	OC_METHODGATE();
	//qDebug()<<"~Remote()";
}

/*! 
   Called by your grandma
 */

void Remote::nodeConfigure()
{
	OC_METHODGATE();
	//qDebug()<<"nodeConfigure()";
	if(mNodeConfigureHelper.configure()) {
		auto ctx=context();
		if(!ctx.isNull()) {

		}
	} else {
		qWarning()<<"ERROR: No context, closing";
		emit nodeRequestExit(EXIT_FAILURE);
	}
}

/*! 
   NODE ACTIVATE LOL
 */

void Remote::nodeActivate(const bool on)
{
	OC_METHODGATE();

	//qDebug()<<"nodeActivate(on="<<on<<")";
	if(on) {
		if(mNodeConfigureHelper.activate(on)) {
			emit nodeActivateChanged(on);
		}
	} else {
		if(mNodeConfigureHelper.isActivatedAsExpected()) {
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
			mWindow = QSharedPointer<RemoteWindow>::create(nullptr);
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

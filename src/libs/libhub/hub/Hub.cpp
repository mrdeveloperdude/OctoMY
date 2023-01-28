#include "Hub.hpp"
#include "HubWindow.hpp"

#include "app/AppContext.hpp"



Hub::Hub()
	: mNodeConfigureHelper("Hub", true, true, false, Constants::OC_LOG_CONFIGURE_HELPER_WARNINGS, Constants::OC_LOG_CONFIGURE_HELPER_CHANGES)

//	: Node(launcher, OC_NEW AppContext(launcher.options(), launcher.environment(), "hub", parent), ROLE_CONTROL, TYPE_HUB, parent)
//AppLauncher<Hub> &launcher, QObject *parent = nullptr
{
	OC_METHODGATE();
	// NOTE: Please do not put code here that generates events. Instead put them in init()
}

Hub::~Hub()
{
	OC_METHODGATE();
}


void Hub::nodeConfigure()
{
	OC_METHODGATE();
	if(mNodeConfigureHelper.configure()) {
		auto ctx=context();
		if(!ctx.isNull()) {
			//QSharedPointer<AgentWindow> mWindow;


			//QSharedPointer<IActuatorController> mActuatorController;

		}
	} else {
		qWarning()<<"ERROR: No context, closing";
		emit nodeRequestExit(EXIT_FAILURE);
	}
}


void Hub::nodeActivate(const bool on)
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


QSharedPointer<NodeWindow> Hub::nodeWindow()
{
	OC_METHODGATE();
	if(mWindow.isNull()) {
		QSharedPointer<Hub> sp=qSharedPointerCast<Hub>(sharedThis());
		if(!sp.isNull()) {
			mWindow=QSharedPointer<HubWindow>(OC_NEW HubWindow(nullptr));
			if(!mWindow.isNull()) {
				mWindow->nodeWindowConfigure(sp);
			} else {
				qWarning()<<"ERROR: Could not allocate AgentWindow";
			}
		} else {
			qWarning()<<"ERROR: Shared pointer to this was null for agent";
		}
	}
	return mWindow;
}


NodeRole Hub::nodeRole()
{
	OC_METHODGATE();
	return ROLE_CONTROL;
}


NodeType Hub::nodeType()
{
	OC_METHODGATE();
	return TYPE_HUB;
}


QSharedPointer<Node> Hub::sharedThis()
{
	OC_METHODGATE();
	return sharedFromThis();
}

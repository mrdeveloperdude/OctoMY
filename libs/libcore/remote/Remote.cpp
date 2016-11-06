#include "Remote.hpp"

#include "../libutil/utility/Standard.hpp"
#include "comms/Client.hpp"
#include "comms/ClientDirectory.hpp"
#include "comms/couriers/DirectPoseCourier.hpp"
#include "RemoteWindow.hpp"
#include "basic/AppContext.hpp"



#include <QDebug>
#include <QDataStream>
#include <QSharedPointer>
#include <QAccelerometerReading>
#include <QGyroscopeReading>
#include <QGeoPositionInfo>


Remote::Remote(NodeLauncher<Remote> &launcher, QObject *parent)
	: Node(new AppContext(launcher.getOptions(), launcher.getEnvironment(), "remote", parent), ROLE_CONTROL, TYPE_REMOTE, parent)
	, poseCourier(new DirectPoseCourier(this))
	, window(nullptr)
{
}

Remote::~Remote()
{
}


QWidget *Remote::showWindow()
{
	if(nullptr==window) {
		window=new RemoteWindow(this, nullptr);
	}
	if(nullptr!=window) {
		window->show();
	}
	return window;
}

///////////////////////////////////////////

void Remote::onDirectPoseChanged(Pose p)
{
	if(0!=poseCourier) {
		poseCourier->setPose(p);
	}
}


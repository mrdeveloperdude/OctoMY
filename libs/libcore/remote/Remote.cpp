#include "Remote.hpp"

#include "../libutil/utility/Standard.hpp"
#include "comms/Client.hpp"
#include "comms/ClientDirectory.hpp"
#include "comms/couriers/DirectPoseCourier.hpp"
#include "remote/RemoteWindow.hpp"
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

Remote::~Remote(){
}

void Remote::start(const NetworkAddress &localAddress, const NetworkAddress &partnerAddress){
	mPartnerAddress=partnerAddress;
	if(nullptr!=mComms){
		mComms->registerCourier(*poseCourier);
		QSharedPointer<Client> c=mComms->getClients()->getByAddress(partnerAddress,true);
		if(nullptr!=c){
			poseCourier->setDestination(c->signature);
			qDebug()<<"comms.start remote "<<localAddress.toString()<< " -> partner "<<partnerAddress.toString();
			mComms->start(localAddress);
		}
		else{
			qWarning()<<"ERROR: could not get client for remote";
		}
	}
}


QWidget *Remote::showWindow(){
	if(nullptr==window){
		window=new RemoteWindow(this, nullptr);
	}
	if(nullptr!=window){
		window->show();
	}
	return window;
}

///////////////////////////////////////////

void Remote::onDirectPoseChanged(Pose p){
	if(0!=poseCourier){
		poseCourier->setPose(p);
	}
}


void Remote::onTouchUpdated(QVector2D v){
	if(nullptr!=mSensorMessage){
		mSensorMessage->touch=v;
		sendStatus();
	}
}

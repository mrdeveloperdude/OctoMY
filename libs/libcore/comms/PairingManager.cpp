#include "PairingManager.hpp"
#include "basic/Standard.hpp"

#include "zoo/ZooClient.hpp"

#include <QDebug>
#include <QDateTime>

/**
  The disciovery and pairing for OctoMY™ nodes are as follows:

	1. Establish physical proximity (in order of preference from security standpoint):
		a. NFC: by having NFC range physical proximity is implied
		b. Cam/QR: by having scanning QR with camera, physical proximity is implied
		c. Bluetooth: by having bluetooth range physical proximity is implied
		d. Zoo: posting exiring gps coords together with pairing signature
		e. LAN: Identify common gateway
	2. Exchange pairing signatures and pubkeys
	3. Exchange pairing challenge/responses to verify legitimity of signature/pubkeys
	4. Show identicon for signatures in list on nodes for user to verify
	5. Rack up multifactors and update list to show the "security level"

	As soon as a node is verified all the way into the list of another
	(aka "discovered"), it's then up to the user to verify that the signatures
	match by inspecting the	respective identicons closely.

	Discovery is done separately from pairing. This means that signatures that
	has been discovered previously will remain discovered regardless if the user
	has selected to trust the signature.

	Further, previously trusted signatures may be dis-trusted later, and then
	re-trusted. The discovery record remains unchanged. Removing discovery
	records is considered an advanced operation that users will not need to
	conduct unless they are in fact debugging the pairing protocol.

*/

static const QDebug &operator<<(QDebug &d, const PairingRole &role){
	switch(role){
		case(ROLE_AGENT):{d.nospace() << "ROLE_AGENT";}break;
		case(ROLE_CONTROL):{d.nospace() << "ROLE_AGENT";}break;
		default: d.nospace() << "ROLE_UNKNOWN";
	}
	return d.maybeSpace();
}

PairingManager::PairingManager(QObject *parent)
	: QObject(parent)
	, role(ROLE_AGENT)
	, zoo(new ZooClient())
	, lastZooPair(0)
	, zooSession(nullptr)
{
	timer.setInterval(2000);
	timer.setTimerType(Qt::VeryCoarseTimer);
	if(!connect(&timer,SIGNAL(timeout()),this,SLOT(onTimer()),OC_CONTYPE)){
		qDebug()<<"ERROR: Could not connect";
	}
}

void PairingManager::start(){
	qDebug()<<"PAIRING MANAGER STARTED";
	timer.start();
}

void PairingManager::stop(){
	qDebug()<<"PAIRING MANAGER STOPPED";
}

void PairingManager::setPin(QString p){
	qDebug()<<"PAIRING MANAGER PIN SET TO "<<p;
	pin=p;
}

void PairingManager::setRole(PairingRole r){
	qDebug()<<"PAIRING MANAGER ROLE SET TO "<<r;
	role=r;
}


const quint64 ZOO_PAIR_INTERVAL=20000;

void PairingManager::onTimer(){
	qDebug()<<"PING";
	if(nullptr!=zoo){
		const quint64 now=QDateTime::currentMSecsSinceEpoch();
		if(now>ZOO_PAIR_INTERVAL+lastZooPair){
			qDebug()<<"ZOO PAIR TIME!";
			lastZooPair=now;
			if(nullptr==zooSession){
				zooSession=new PairingSession();
				qDebug()<<"ZOO PAIR SESSION CREATED!";
			}
			if(nullptr!=zooSession){
				if(!zooSession->isComplete()){
					zoo->doPairingEscrow("1234",[=](bool s, QVariantMap m){
						qDebug()<<"PAITING ESCROW DONE WITH : "<<s;
						if(zooSession->isComplete()){
							qDebug()<<"ZOO PAIR SESSION COMPLETED!";
						}
					});
				}
			}
		}
	}
}

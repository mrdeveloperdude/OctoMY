#include "DiscoveryClient.hpp"

#include "basic/Standard.hpp"

#include "zoo/ZooClient.hpp"
#include "zoo/ZooConstants.hpp"

#include "security/KeyStore.hpp"

#include "comms/discovery/DiscoveryParticipant.hpp"

#include "basic/Node.hpp"
#include "utility/Utility.hpp"

#include <functional>
#include "../libqhttp/qhttpclient.hpp"
#include "../libqhttp/qhttpclientrequest.hpp"
#include "../libqhttp/qhttpclientresponse.hpp"



#include <QDebug>
#include <QDateTime>
#include <QJsonDocument>


/**
  The discovery and pairing for OctoMY™ nodes are as follows:

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
	has selected to trust them or not. In fact the user is free to
	trust/dis-trust/re-trust any of the discovered nodes as many times
	as she likes.

	Removing discovery records is considered an advanced operation that users
	will not need to conduct unless they are in fact debugging the pairing protocol.


	Glossary:

	+ Discovery: A comlpetely automatic process whereby nodes find eachother and
	  exchange the necessary details to be able to unmistakably verify the
	  identity of eachother in the future. May be aided by manual process like
	  camera discovery. NOTE: discovery is the hard part, pairing is just UI.

	+ Pairing: A manual process whereby the operator assigns to a previously
	  dicovered node any amount of trust.

	+ Node: Any participant in an OctoMY network, such as agent, hub or remote.

	+ Birth/Delivery: The transition of a node from uninitialized to initialized.
	  Before the node is initialized, it does not have the necessary
	  identification and security primitives to parttake in discovery and pairing.


*/


DiscoveryClient::DiscoveryClient(Node &node)
	: QObject(&node)
	, lastZooPair(0)
	, m_serverURL("http://localhost:8123/api")
	, m_client(new qhttp::client::QHttpClient(this))
	, node(node)
	, ourPubKey(node.getKeyStore().getLocalPublicKey())
	, ourID(utility::toHash(ourPubKey))
	, zeroID(utility::toHash(""))
{
	timer.setInterval(500);
	timer.setTimerType(Qt::VeryCoarseTimer);
	if(!connect(&timer,SIGNAL(timeout()),this,SLOT(onTimer()),OC_CONTYPE)){
		qDebug()<<"ERROR: Could not connect";
	}
}

void DiscoveryClient::start(){
	qDebug()<<"DISCOVERY CLIENT STARTED";
	if(!timer.isActive()){
		onTimer();
	}
	timer.start();
}

void DiscoveryClient::stop(){
	qDebug()<<"DISCOVERY CLIENT STOPPED";
	timer.stop();
}



void DiscoveryClient::discover(){
	qDebug()<<"DISCOVERY CLIENT RUN";

	qhttp::client::TRequstHandler reqHandler= [this](qhttp::client::QHttpRequest* req){
		QVariantMap cmd;
		cmd["action"] = ZooConstants::OCTOMY_ZOO_API_DO_DISCOVERY_ESCROW;
		cmd["publicKey"] = ourPubKey;
		cmd["localAddress"] = "10.0.0.3";
		cmd["localPort"] = 12345;
		cmd["publicPort"] = 54321;
		cmd["manualPin"] ="12345";
		cmd["role"] = DiscoveryRoleToString(node.getRole());
		cmd["type"] = DiscoveryTypeToString(node.getType());
		QByteArray body  = QJsonDocument::fromVariant(cmd).toJson();
		//qDebug()<<"SENDING RAW JSON: "<<body;
		req->addHeader("user-agent",			ZooConstants::OCTOMY_USER_AGENT);
		req->addHeader(ZooConstants::OCTOMY_API_VERSION_HEADER,		ZooConstants::OCTOMY_API_VERSION_CURRENT);
		req->addHeader("accept",				"application/json");
		req->addHeader("content-type",			"application/json");
		req->addHeader("connection",			"keep-alive");
		req->addHeaderValue("content-length", 	body.length());
		req->end(body);
		//qDebug()<<"Getting node by OCID:"<<OCID << " REQ END";
	};

	qhttp::client::TResponseHandler resHandler=	[this](qhttp::client::QHttpResponse* res) {
		//qDebug()<<"Getting node by OCID:"<<OCID << " RES";
		res->collectData(10000);
		res->onEnd([this, res](){
			qhttp::TStatusCode status=res->status();
			bool ok=true;
			QString message="";
			if(qhttp::ESTATUS_OK!=status){
				ok=false;
				message="ERROR: HTTP Code was "+QString::number(status)+" instead of 200 OK: ";
			}
			else{
				//qDebug()<<"Getting node by OCID:"<<OCID<<" RES: ON END";
				QJsonDocument doc = QJsonDocument::fromJson(res->collectedData());
				QByteArray data=doc.toJson();
				QVariantMap root = QJsonDocument::fromJson(data).toVariant().toMap();
				if ( root.isEmpty() ) {
					qWarning() << "ERROR: The result is an invalid json";
					qWarning() << "ERROR: OFFENDING JSON IS: "<<data;
					message="ERROR: invalid json body in response";
					ok=false;
				}
				else{
					qDebug()<<"RETURNED STATUS WAS: "<<root.value("status").toString()<<", MSG:  "<<root.value("message").toString();
					if(root.contains("participants")){
						qDebug()<<"PARTICIPANTS: "<<root.value("participants");
						QVariantList partList=root.value("participants").toList();
						for(QVariant part:partList){
							QVariantMap map=part.toMap();
							registerPossibleParticipant(map);
						}
					}
					bool rok=("ok"==root.value("status").toString());
					if(!rok){
						message=root.value("message").toString();
					}
					ok=ok && rok;
				}
			}
			qDebug()<<"STATUS("<<ok<<"):"<<message;
		});
		//qDebug()<<"Getting node by OCID:"<<OCID << " RES DONE";
	};
	m_client->request(qhttp::EHTTP_POST, m_serverURL, reqHandler, resHandler);


}

void DiscoveryClient::registerPossibleParticipant(QVariantMap map){
	const QString partID=utility::toHash(map["publicKey"].toString());
	// Is this a genuine participant?
	//qDebug()<<"ZERO: "<<zeroID<<" OUR: "<<ourID<<" PART:"<<partID;
	if(partID!=zeroID && ourID!=partID){
		DiscoveryClientStore &peers=node.getPeers();
		//TODO: Scrutinize newcomer for security
		DiscoveryParticipant *old=nullptr;
		if(peers.hasParticipant(partID)){
			old=peers.getParticipant(partID);
			qDebug()<<" + Found replacement participant:"<<partID;
		}
		else{
			qDebug()<<" + Found new participant:"<<partID;
		}
		DiscoveryParticipant *p=new  DiscoveryParticipant(map);
		if(p->isValidClient()){
			peers.setParticipant(p);
			delete old;
			emit nodeDiscovered(partID);
		}
		else{
			qDebug()<<" + Participant failed validity test, skipping:"<<partID;
		}
	}
	else{
		qDebug()<<" + Skipping invalid new participant:"<<partID;
	}
}

Node &DiscoveryClient::getNode()
{
	return node;
}

/*
QMap<QString, DiscoveryParticipant *> &DiscoveryClient::getParticipants(){
	return participants;
}*/

const quint64 ZOO_PAIR_INTERVAL=20000;

void DiscoveryClient::onTimer(){
	//qDebug()<<"PING";
	const quint64 now=QDateTime::currentMSecsSinceEpoch();
	if(now>ZOO_PAIR_INTERVAL+lastZooPair){
		qDebug()<<"ZOO PAIR TIME!";
		lastZooPair=now;
		//discover();
	}
	discover();
}

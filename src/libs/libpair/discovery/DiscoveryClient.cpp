#include "DiscoveryClient.hpp"

#include "DiscoveryMandate.hpp"
#include "address/Associate.hpp"
#include "app/Constants.hpp"
#include "client/zoo/ZooClient.hpp"
#include "comms/Comms.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/couriers/DiscoveryCourier.hpp"
#include "discovery/AddressBook.hpp"
#include "node/Node.hpp"
#include "qhttp/qhttpclient.hpp"
#include "qhttp/qhttpclientrequest.hpp"
#include "qhttp/qhttpclientresponse.hpp"
#include "security/KeyStore.hpp"
#include "security/SecurityConstants.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/SharedPointerWrapper.hpp"
#include "utility/data/Data.hpp"
#include "utility/string/String.hpp"
#include "utility/time/HumanTime.hpp"

#include <QDebug>
#include <QDateTime>
#include <QJsonDocument>



static const QString zeroID=utility::string::toHash("", OCTOMY_KEY_HASH);



DiscoveryClient::DiscoveryClient()
	: QObject(nullptr)
	, mLastZooPair(0)
	, mZooClient(OC_NEW qhttp::client::QHttpClient(this))
	, mDebug(false)
	, mRXRate("RX")
	, mTXRate("TX")
	, mConfigureHelper("DiscoveryClient", true, true, false, true, false)
{
	OC_METHODGATE();
}


DiscoveryClient::~DiscoveryClient()
{
	OC_METHODGATE();
}


void DiscoveryClient::configure(QSharedPointer<Node> node, quint64 triggeredInterval, quint64 idleInterval, quint64 triggeredTime, quint64 decayTime)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode=node;
		if(mNode.isNull()) {
			qWarning() << "ERROR: node was null";
		}
		mHoneymoonScheduler.configure(triggeredInterval, triggeredTime, decayTime, idleInterval);
		mTimer.setInterval(500);
		mTimer.setTimerType(Qt::VeryCoarseTimer);
		if(!connect(&mTimer, SIGNAL(timeout()), this, SLOT(onTimer()), OC_CONTYPE)) {
			qWarning() << "ERROR: Could not connect";
		}
	}
}


void DiscoveryClient::activate(bool on)
{
	OC_METHODGATE();
	if(mConfigureHelper.activate(on)) {
		if(mDebug) {
			qDebug().noquote().nospace() << "DISCOVERY CLIENT " << QString(on?"ACTIVATED":"DEACTICVATED") << "WITH MANDATE" << mMandate;
		}
		if(on) {
			mHoneymoonScheduler.trigger();
			if(!mTimer.isActive()) {
				onTimer();
			}
			mTimer.start();
		} else {
			mHoneymoonScheduler.reset();
			mTimer.stop();
		}
	}
}


bool DiscoveryClient::isActive() const
{
	OC_METHODGATE();
	return mTimer.isActive();
}


QSharedPointer<Key> DiscoveryClient::localKey() const
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		QSharedPointer<KeyStore> ks = mNode->keyStore();
		if(!ks.isNull()) {
			return ks->localKey();
		}
	}
	return nullptr;
}


void DiscoveryClient::setZooURL(const QUrl& serverURL)
{
	OC_METHODGATE();
	mZooURL  = serverURL;
	if(mDebug) {
		qDebug().noquote().nospace() << "DISCOVERY CLIENT	Using new URL: " << mZooURL;
	}
}


QUrl DiscoveryClient::zooURL() const
{
	OC_METHODGATE();
	return mZooURL;
}


void DiscoveryClient::setMandate(QSharedPointer<const DiscoveryMandate> mandate){
	OC_METHODGATE();
	mMandate = mandate;
}


QSharedPointer<const DiscoveryMandate> DiscoveryClient::mandate() const{
	OC_METHODGATE();
	return mMandate;
}


const RateCalculator &DiscoveryClient::rxRate() const{
	return mRXRate;
}


const RateCalculator &DiscoveryClient::txRate() const{
	return mTXRate;
}


void DiscoveryClient::requestHandler(qhttp::client::QHttpRequest* req){
	QSharedPointer<Key> key = localKey();
	if(!key.isNull()) {
		QVariantMap cmd;
		cmd["action"] = Constants::OCTOMY_ZOO_API_DO_DISCOVERY_ESCROW;
		cmd["key"] = key->toVariantMap(true);
		cmd["manualPin"] = "12345";
		if(!mNode.isNull()) {
			auto nodeIdentity = mNode->nodeIdentity();
			if(!nodeIdentity.isNull()) {
				auto map = nodeIdentity->toVariantMap();
				utility::data::merge(cmd, map);
			} else {
				qWarning() << "ERROR: no nodeIdentity";
			}
			
			{
				auto al = mNode->localAddressList();
				if(!al.isNull()) {
					QVariantList alist;
					alist << al->currentCarrierAddress()->toVariantMap();
					cmd["addressList"] = alist;
				} else {
					qWarning() << "ERROR: no local addressList";
				}
			}
			
		} else {
			qWarning() << "ERROR: no mNode";
		}
		
		if(mDebug) {
			qDebug().noquote().nospace() << "SENDING VAR: " << cmd;
		}
		auto jdoc=QJsonDocument::fromVariant(cmd);
		if(mDebug) {
			qDebug().noquote().nospace() << "SENDING JDOC: " << jdoc;
		}
		QByteArray body = jdoc.toJson(QJsonDocument::Indented);
		if(mDebug) {
			qDebug().noquote().nospace() << "SENDING RAW JSON: " << body;
		}
		req->addHeader("user-agent",			Constants::OCTOMY_USER_AGENT);
		req->addHeader(Constants::OCTOMY_API_VERSION_HEADER,		Constants::OCTOMY_API_VERSION_CURRENT);
		req->addHeader("accept",				"application/json");
		req->addHeader("content-type",			"application/json");
		req->addHeader("connection",			"keep-alive");
		req->addHeaderValue("content-length", 	body.length());
		req->end(body);
		mTXRate.countPacket(static_cast<quint32>(body.size()));
	} else {
		qWarning() << "ERROR: no key";
	}
}


void DiscoveryClient::responseHandler(qhttp::client::QHttpResponse* res){
	if(mDebug) {
		qDebug().noquote().nospace() << "RES";
	}
	res->collectData(20000);
	res->onEnd([this, res]() {
		//qDebug() << "RES to " << m_serverURL;
		//qDebug() << "RES END";
		auto status = res->status();
		auto ok{true};
		QString message;
		if(qhttp::ESTATUS_OK != status) {
			ok = false;
			message = "ERROR: HTTP Code was " + QString::number(status) + " instead of 200 OK";
		} else {
			auto raw=res->collectedData();
			auto doc = QJsonDocument::fromJson(raw);
			mRXRate.countPacket(static_cast<quint32>(raw.size()));
			auto data = doc.toJson();
			auto root = QJsonDocument::fromJson(data).toVariant().toMap();
			if ( root.isEmpty() ) {
				qWarning() << "ERROR: The result is an invalid json";
				qWarning() << "ERROR: OFFENDING JSON IS: " << data;
				message = "ERROR: invalid json body in response";
				ok = false;
			} else {
				//qDebug() << "RETURNED STATUS WAS: " << root.value("status").toString() << ", MSG:  " << root.value("message").toString();
				bool rok=("ok"==root.value("status").toString());
				if(!rok) {
					message=root.value("message").toString();
				}
				ok = ok && rok;
				if(ok) {
					if(root.contains("peers")) {
						//qDebug() << "ASSOCIATES: " << root.value("peers");
						auto partList = root.value("peers").toList();
						if(partList.size()<=0) {
							qWarning() << " + Associate count was 0 in response";
							message = "ERROR: Associate count was 0 in response";
							ok = false;
						}
						//qWarning() << " + Got " << partList.size() << " associates";
						for(auto &part:partList) {
							auto map = part.toMap();
							registerPossibleAssociate(map);
						}
					} else {
						qWarning() << " + No associates in response";
						message = "ERROR: No associates in response";
						ok = false;
					}
				} else {
					qWarning() << " + Not OK. DATA: " << data;
				}
			}
		}
		if(!ok) {
			if(mDebug) {
				qDebug().noquote().nospace() << "RETURNED STATUS (" << ok << "):" << message;
			}
		}
		emit discoverResponse(ok);
	});
	if(mDebug) {
		qDebug().noquote().nospace() << "Getting node: RES DONE";
	}
}


void DiscoveryClient::discoverZoo(){
	OC_METHODGATE();
	if(mZooURL.isEmpty()){
		qWarning().noquote().nospace() << "Invalid discovery server URL: '" << mZooURL << "'";
		emit discoverResponse(false);
		return;
	}
	if(mDebug) {
		qDebug().noquote().nospace() << "DISCOVERY CLIENT Zoo discovery for '" << mZooURL << "'";
	}
	qhttp::client::TRequstHandler reqHandler = [this](qhttp::client::QHttpRequest* req) {
		requestHandler(req);
	};
	
	qhttp::client::TResponseHandler resHandler = [this](qhttp::client::QHttpResponse* res) {
		responseHandler(res);
	};
	const auto ret = mZooClient->request(qhttp::EHTTP_POST, mZooURL, reqHandler, resHandler);
	if(!ret) {
		qWarning().noquote().nospace() << "ERROR: Could not execute discovery request to '" << mZooURL << "'";
		emit discoverResponse(false);
	}
}


void DiscoveryClient::discoverNetwork(){
	OC_METHODGATE();
	
}


void DiscoveryClient::discoverBluetooth(){
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		
	}
}


void DiscoveryClient::discover()
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		emit discoverRequest();
		if(mMandate.isNull()){
			qWarning().noquote().nospace() << "Invalid discovery mandate";
			emit discoverResponse(false);
			return;
		}
		const auto &m{*mMandate};
		if(m.useZoo){
			discoverZoo();
		}
	}
}


void DiscoveryClient::registerPossibleAssociate(QVariantMap map)
{
	OC_METHODGATE();
	if(mConfigureHelper.isActivatedAsExpected()) {
		if(mDebug) {
			qDebug().noquote().nospace() << "REG";
		}
		if(mNode.isNull()) {
			qWarning() << "ERROR: no mNode";
			return;
		}
		auto key = QSharedPointer<Key>::create(map["key"].toMap(), true);
		if(!key.isNull()) {
			auto keyStore = mNode->keyStore();
			auto ourKey = keyStore->localKey();
			if(!ourKey.isNull()) {
				const auto partID = key->id().trimmed();
				const auto ourID = ourKey->id();
				if("" == partID) {
					qWarning() << "ERROR: Skipping new participant with empty ID: " << partID;
				} else if(zeroID == partID) {
					qWarning() << "ERROR: Skipping new participant with zero ID: " << partID;
				} else if(ourID == partID) {
					//qDebug() << " + Skipping new participant with our ID: " << partID;
				} else {
					auto peers = mNode->addressBook();
					if(!peers.isNull()) {
						QSharedPointer<Associate> part;
						if(peers->hasAssociate(partID)) {
							if(mDebug) {
								qDebug() << " + Updating participant with ID: " << partID;
							}
							part = peers->associateByID(partID);
							if(part->update(map.value("associate").toMap(), false)) {
								emit peers->associatesChanged();
								emit nodeDiscovered(partID);
							}
						} else {
							part = QSharedPointer<Associate>::create(map.value("associate").toMap());
							if(!part.isNull()) {
								if(part->isValidForClient()) {
									auto comms = mNode->comms();
									if(!comms.isNull()) {
										auto courier = QSharedPointer<DiscoveryCourier>::create(part, comms);
										if(!courier.isNull()) {
											// SECURITY: What if the current associate has lots of important data? we need a smart merge instead of this.
											peers->upsertAssociate(part);
											courier->setDestination(part->id());
											comms->registerCourier(courier, true);
											if(mDebug) {
												qDebug() << " + Adding new participant with ID: " << partID;
											}
											emit nodeDiscovered(partID);
										} else {
											qWarning() << "ERROR: Could not create courier for part with ID " << partID;
										}
									} else {
										qWarning() << "ERROR: Node had no comms";
									}
								} else {
									if(mDebug) {
										qDebug() << " + Skipping invalid new participant:" << partID;
									}
								}
							} else {
								qWarning() << "ERROR: Could not allocate participant: " << partID;
							}
						}
						// Update the key while we are at it...
						// SECURITY: Check for security implications
						keyStore->setPubKeyForID(map["key"].toMap()["publicKey"].toString());
					} else {
						qWarning() << "ERROR: no mpeers";
						return;
					}
				}
			} else {
				qWarning() << "ERROR: no ourKey";
			}
		} else {
			qWarning() << "ERROR: no key";
		}
	}
}


QSharedPointer<Node> DiscoveryClient::node() const
{
	OC_METHODGATE();
	return mNode;
}


quint64 DiscoveryClient::lastZooPairTime() const
{
	OC_METHODGATE();
	return mLastZooPair;
}


void DiscoveryClient::setLogging(const bool logging)
{
	OC_METHODGATE();
	mDebug=logging;
}


bool DiscoveryClient::isLogging() const
{
	OC_METHODGATE();
	return mDebug;
}


void DiscoveryClient::onTimer()
{
	OC_METHODGATE();
	const quint64 now = utility::time::currentMsecsSinceEpoch<quint64>();
	const quint64 interval = mHoneymoonScheduler.currentValue(now);
	if(now > (interval + mLastZooPair)) {
		//qDebug() << "ZOO PAIR TIME WITH INTERVAL " << interval;
		mLastZooPair = now;
		discover();
		//TODO: node.getComms()->unregisterCourier(courier); <-- remove old unused and timed out couriers
	}
}

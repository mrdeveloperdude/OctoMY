#include "Comms.hpp"

#include "address/AddressEntry.hpp"
#include "comms/CommsChannel.hpp"
#include "comms/CommsSession.hpp"
#include "comms/Multimagic.hpp"
#include "comms/carriers/CommsCarrierUDP.hpp"
#include "discovery/AddressBook.hpp"
#include "security/KeyStore.hpp"
#include "uptime/Assert.hpp"
#include "uptime/MethodGate.hpp"

static const qint64 MS_PER_SEC{1000};

Comms::Comms(QObject *parent)
	: QObject(parent)
	, mConfigureHelper("Comms", true, false, false, true, false)
{
	OC_METHODGATE();
}



void Comms::configure( QSharedPointer<KeyStore> keystore, QSharedPointer<AddressBook> peers){
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		
		mCarrierUDP = QSharedPointer<CommsCarrierUDP>::create();
		if(mCarrierUDP){
			mCarrierUDP->configure();
		}
		else{
			qWarning() << "Could not create UDP Carrier";
		}
		
		
		mChannelUDP = QSharedPointer<CommsChannel>::create();
		if(mChannelUDP){
			mChannelUDP->configure(sharedFromThis(), mCarrierUDP);
		}
		else{
			qWarning() << "Could not create UDP Channel";
		}
		
		// Send a packet every 1 second untill grace period runs out at which point we are down to on per 60 seconds
		mHoneymoon.configure(1 * MS_PER_SEC, 10 * MS_PER_SEC, 20 * MS_PER_SEC, 60 * MS_PER_SEC);
	}
}

bool Comms::activate(bool activeWanted)
{
	OC_METHODGATE();
	bool success=true;
	if(mConfigureHelper.activate(activeWanted)) {
		if(!activeWanted) {
			mCarrierUDP->activate(activeWanted);
			
			
			/// stuff from carrier service:
			///
			
			/*
		if(nullptr != carrier) {
			auto localAddressList=mNode->localAddressList();
			if(!localAddressList.isNull()) {
				// TODO: Handle any address type
				const auto listenAddress=QSharedPointer<CarrierAddressUDP>::create(QHostAddress::Any, localAddressList->port());
				carrier->setListenAddress(listenAddress);
				ok=carrier->activate(on);
			}			 
*/
			// Stuff from comms service:
			//comms->activate(on);
			
		}
	} else {
		success=false;
	}
	return success;
}




// [Un]register courier to NODE
void Comms::registerCourier(QSharedPointer<Courier> courier, bool reg)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
		//qDebug()<<"SETTING COURIER "<<courier->toString()<<" TO "<<(reg?"REGISTERED":"UNREGISTERED");
		if(!courier.isNull()) {
			mChannelUDP->registerCourier(courier, reg);
			/*
			const bool ok=mCouriers.registerCourier(courier, reg);
			if(ok) {
				const quint32 id=courier->id();
				if(reg) {
					mCouriersByID[id]=courier;
				} else {
					mCouriersByID.remove(id);
				}
			}
			hookCourierSignals(courier, (ok || !reg));
*/
		}
		
		// Make sure connection status is updated after courrier count changed
		//updateConnection();
	}
}


CommsSessionDirectory &Comms::sessions()
{
	OC_METHODGATE();
	return mSessions;
}


QSharedPointer<KeyStore> Comms::keyStore() const{
	OC_METHODGATE();
	return mKeystore;
}



QSharedPointer<CommsSession> Comms::sessionByAssociateID(const QString &id)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		// Look for existing sessions tied to this ID
		return mSessions.byFullID(id);
	}
	return QSharedPointer<CommsSession>();
}



QSharedPointer<CommsSession> Comms::sessionBySessionID(const SESSION_ID_TYPE &id)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		// Look for existing sessions tied to this ID
		return mSessions.bySessionID(id);
	}
	return QSharedPointer<CommsSession>();
}



QSharedPointer<Associate> Comms::associateByID(const QString &id)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		return mAssociates->associateByID(id);
	}
	return QSharedPointer<Associate>();
}

QSharedPointer<Courier> Comms::courierByID(const quint32 &id){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		return mCouriersByID.value(id, nullptr);
	}
	return QSharedPointer<Courier>();
}



void Comms::triggerHoneymoon(const quint64 now){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		mHoneymoon.trigger(now);
	}
}

quint64 Comms::honeymoonValue(const quint64 now){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		return mHoneymoon.currentValue(now);
	}
	return 0;
}


QSharedPointer<CommsSession> Comms::createSession(const QString &id, const bool &initiator)
{
	OC_METHODGATE();
	auto &sessions = this->sessions();
	const auto &keystore= this->keyStore();
	QSharedPointer<CommsSession> session(nullptr);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(keystore->hasPubKeyForID(id)) {
			auto key=keystore->pubKeyForID(id);
			if(nullptr!=key) {
				if(key->isValid(true)) {
					auto localSessionID = sessions.generateUnusedSessionID();
					if(localSessionID >= FIRST_STATE_ID ) {
						QSharedPointer<Associate> associate=mAssociates->associateByID(id);
						if(nullptr!=associate) {
							auto ae=associate->addressList().highestScore();
							if(!ae.isNull()) {
								auto addr = ae->address;
								if(addr->isValid()) {
									session = QSharedPointer<CommsSession>::create(key);
									OC_ASSERT(nullptr!=session);
									if(nullptr!=session) {
										//session->setRemoteSessionID(desiredRemoteSessionID);
										session->handshakeState().setInitiator(initiator);
										session->setLocalSessionID(localSessionID);
										
										
										//associate->localAddress()
										
										session->setAddress(addr);
										// Generate our syn nonce
										//const auto synNonce = session->createOurSynNonce();
										//qDebug()<<"OUR SYN TX NONCE CREATED: "<<synNonce;
										
										sessions.insert(session);
										//qDebug()<< "NEW SESSION CREATED FOR ID "<<id<< " with local sessionID "<<QString::number(localSessionID)<< (initiator?"INITIATED":"");
										
										// TODO: Should we emit commsClientAdded here?
										
										//qDebug()<<"CREATE SESSION RETURNING VALUE: "<<session->summary();
										/*
										auto k=session->key();
										if(nullptr!=k) {
											qDebug()<<"IN:  KEY PTR="<<(void *)(k.data())<<", DKEY PTR="<<(void *)(k->d_func_dbg() );
										} else {
											qDebug()<<"IN:  NO KEY";
										}
										*/
										
									} else {
										QString es="ERROR: OctoMY Protocol Could not create session for sender with ID "+id+", could not allocate";
										qWarning()<<es;
										emit commsError(es);
									}
								} else {
									qWarning()<<"ERROR: address was invalid when creating comms session for ID "+id;
								}
							} else {
								qWarning()<<"ERROR: address was null when creating comms session for ID "+id;
							}
						} else {
							QString es="ERROR: no associate found for ID "+id;
							qWarning()<<es;
							emit commsError(es);
						}
					} else {
						QString es="ERROR: OctoMY Protocol local session ID not valid: "+QString::number(localSessionID);
						qWarning()<<es;
						emit commsError(es);
					}
				} else {
					QString es="ERROR: OctoMY Protocol Could not create session for sender with ID "+id+", key was invalid";
					qWarning()<<es;
					emit commsError(es);
				}
			} else {
				QString es="ERROR: no key";
				qWarning()<<es;
				emit commsError(es);
			}
		} else {
			//keystore.dump();
			QString es=QStringLiteral("ERROR: OctoMY Protocol Session-Less sender '")+id+QStringLiteral("' unknown. (Keystore.ready=")+QString(keystore->ready()?"YES":"NO")+")";
			qWarning().noquote()<<es;
			emit commsError(es);
		}
	}
	return session;
}

	
// Report if this commschannel would rather be connected or not (registered couriers > 0 and activated)
bool Comms::needConnection()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		// TODO: Adapt this from CommsChannel
		//return ( (mCouriers.count()>0) && (mConfigureHelper.isActivated()) );
	}
	return false;
}


// [Dis]connect based on our needConnection()
void Comms::updateConnection()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		/* TODO: Adapt this from CommsChannel
		if(!mCarrier.isNull()) {
			const bool needsConnection=needConnection();
			mCarrier->maintainConnection(needsConnection);
		}
*/
	}
}

CourierSet Comms::couriers(){
	OC_METHODGATE();
	auto ret = CourierSet();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		ret += mChannelUDP->couriers();
	}
	return ret;
}

bool Comms::audioOn() const {
	OC_METHODGATE();

}


bool Comms::cameraOn() const {
	OC_METHODGATE();

}


bool Comms::bluetoothOn() const {
	OC_METHODGATE();

}


bool Comms::NFCOn() const {
	OC_METHODGATE();
}


bool Comms::networkOn() const {
	OC_METHODGATE();
	if(mCarrierUDP){
		return mCarrierUDP->isConnectionMaintained();
	}
	return false;
}


bool Comms ::zooOn() const {
	OC_METHODGATE();
	return false;
}




void Comms::toggleAudio(const bool on)
{
	OC_METHODGATE();
	qDebug()<<"AU";
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}

void Comms::toggleBluetooth(const bool on)
{
	OC_METHODGATE();
	qDebug()<<"BT";
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}

void Comms::toggleNetwork(const bool on)
{
	OC_METHODGATE();
	qDebug()<<"NT";
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(mCarrierUDP){
			mCarrierUDP->maintainConnection(on);
		}
		else{
			qWarning() << "No UDP carrier";
		}
	}
}


void Comms::toggleZoo(bool on){
	OC_METHODGATE();
	qDebug()<<"ZO";
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}

/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "CommsCarrierUDP.hpp"

#include "comms/address/CarrierAddressUDP.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "utility/string/String.hpp"

// It seems that 30 seconds would be a "safe" minimal UDP rate to avoid routers closing our "connection"
#define OCTOMY_UDP_MAXIMAL_PACKET_RATE (1000)
#define OCTOMY_UDP_MINIMAL_PACKET_RATE (5000)


// NOTE: We use 512 as the maximum practical UDP size for ipv4 over the internet
//       See this for discussion: http://stackoverflow.com/questions/1098897/what-is-the-largest-safe-udp-packet-size-on-the-internet
const qint32 CommsCarrierUDP::MAX_UDP_PAYLOAD_SIZE = 512;

CommsCarrierUDP::CommsCarrierUDP(QObject *parent)
	: CommsCarrier(parent)

{
	OC_METHODGATE();
	setObjectName("CommsCarrierUDP");

	if(!QObject::connect(&mUDPSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()), OC_CONTYPE)) {
		qWarning()<<"Could not connect UDP readyRead";
	}
	
	// TODO: Handle missing error signal
	qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
	if(!QObject::connect(&mUDPSocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)), OC_CONTYPE)) {
		qWarning()<<"Could not connect UDP error";
	}
}



CommsCarrierUDP::~CommsCarrierUDP()
{
	OC_METHODGATE();
	mUDPSocket.waitForDisconnected();
}


//////////////////////////////////////////////////


void CommsCarrierUDP::onReadyRead()
{
	OC_METHODGATE();
	emit  carrierReadyRead();
}

void CommsCarrierUDP::onError(QAbstractSocket::SocketError errorCode)
{
	OC_METHODGATE();
	emit carrierError(utility::string::socketErrorToString(errorCode));
}




// CommsCarrier internal interface methods
//////////////////////////////////////////////////


void CommsCarrierUDP::configureImp()
{
	OC_METHODGATE();
}

bool CommsCarrierUDP::activateImp(const bool on)
{
	OC_METHODGATE();
	bool success=true;
	if(on) {
		success = (!mLocalAddressUDP.isNull()) && mUDPSocket.bind(mLocalAddressUDP->ip(), mLocalAddressUDP->port());
		//qDebug()<<"----- comms bind "<< mLocalAddress.toString()<< " with interval "<<utility::string::humanReadableElapsedMS(mSendingTimer.interval()) <<(success?" succeeded": (" failed with '"+mUDPSocket.errorString()+"'") );

	} else {
		mUDPSocket.close();
		//qDebug()<<"----- comms unbind "<< mLocalAddress.toString();
	}
	return success;
}


void CommsCarrierUDP::setAddressImp(QSharedPointer<CarrierAddress> address)
{
	OC_METHODGATE();
	mLocalAddressUDP = qSharedPointerCast<CarrierAddressUDP>(address);
}


bool CommsCarrierUDP::isActiveImp() const
{
	OC_METHODGATE();
	return (QAbstractSocket::BoundState == mUDPSocket.state());
}


qint64 CommsCarrierUDP::writeDataImp(const QByteArray &datagram, QSharedPointer<CarrierAddress> toAddress)
{
	OC_METHODGATE();
	auto toAddressUDP = qSharedPointerCast<CarrierAddressUDP>(toAddress);
	if(toAddressUDP.isNull()){
		if(toAddress.isNull()){
			qWarning()<<"ERROR: No to-address specified";
		}
		else{
			qWarning()<<"ERROR: To-address wrong type: " << CarrierAddressTypeToString(toAddress->type());
		}
		return -1;
	}
	const qint64 ret = mUDPSocket.writeDatagram(datagram, toAddressUDP->ip(), toAddressUDP->port());
	if(ret<0) {
		qWarning()<<"ERROR: Writing data to UDB socket: "<<mUDPSocket.errorString();
	}
	return ret;
}

qint64 CommsCarrierUDP::readDataImp(char *data, qint64 maxlen, QHostAddress *host, quint16 *port)
{
	OC_METHODGATE();
	const qint64 ret=mUDPSocket.readDatagram(data, maxlen, host, port);
	if(ret<0) {
		qWarning()<<"ERROR: Reading data from UDB socket: "<<mUDPSocket.errorString();
	}
	return ret;
}

bool CommsCarrierUDP::hasPendingDataImp()
{
	OC_METHODGATE();
	return mUDPSocket.hasPendingDatagrams();
}

qint64 CommsCarrierUDP::pendingDataSizeImp()
{
	OC_METHODGATE();
	return mUDPSocket.pendingDatagramSize();
}


QString CommsCarrierUDP::errorStringImp()
{
	OC_METHODGATE();
	return mUDPSocket.errorString();
}


QSharedPointer<CarrierAddress> CommsCarrierUDP::addressImp()
{
	OC_METHODGATE();
	return qSharedPointerCast<CarrierAddress>(mLocalAddressUDP);
}


quint64 CommsCarrierUDP::minimalPacketIntervalImp()
{
	OC_METHODGATE();
	return OCTOMY_UDP_MINIMAL_PACKET_RATE;
}


quint64	CommsCarrierUDP::maximalPacketIntervalImp()
{
	OC_METHODGATE();
	return OCTOMY_UDP_MAXIMAL_PACKET_RATE;
}

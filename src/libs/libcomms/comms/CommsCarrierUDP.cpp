/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "CommsCarrierUDP.hpp"

#include "utility/Standard.hpp"
#include "utility/Utility.hpp"


// It seems that 30 seconds would be a "safe" minimal UDP rate to avoid routers closing our "connection"
#define OCTOMY_UDP_MAXIMAL_PACKET_RATE (1000)
#define OCTOMY_UDP_MINIMAL_PACKET_RATE (5000)


// NOTE: We use 512 as the maximum practical UDP size for ipv4 over the internet
//       See this for discussion: http://stackoverflow.com/questions/1098897/what-is-the-largest-safe-udp-packet-size-on-the-internet
const qint32 CommsCarrierUDP::MAX_UDP_PAYLOAD_SIZE = 512;

CommsCarrierUDP::CommsCarrierUDP(QObject *parent)
	: CommsCarrier(parent)

{
	setObjectName("CommsCarrierUDP");

	if(!connect(&mUDPSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()), OC_CONTYPE)) {
		qWarning()<<"Could not connect UDP readyRead";
	}
	qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
	if(!connect(&mUDPSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)), OC_CONTYPE)) {
		qWarning()<<"Could not connect UDP error";
	}

}



CommsCarrierUDP::~CommsCarrierUDP()
{

}


//////////////////////////////////////////////////


void CommsCarrierUDP::onReadyRead()
{
	emit  carrierReadyRead();
}

void CommsCarrierUDP::onError(QAbstractSocket::SocketError errorCode)
{
	emit carrierError(utility::socketErrorToString(errorCode));
}




// CommsCarrier internal interface methods
//////////////////////////////////////////////////



bool CommsCarrierUDP::startImp(NetworkAddress address)
{

	mLocalAddress=address;
	const bool b = mUDPSocket.bind(mLocalAddress.ip(), mLocalAddress.port());
	qDebug()<<"----- comms bind "<< mLocalAddress.toString()<< " with interval "<<utility::humanReadableElapsedMS(mSendingTimer.interval()) <<(b?" succeeded": " failed");
	return b;
}

void CommsCarrierUDP::stopImp()
{
	mUDPSocket.close();
	qDebug()<<"----- comms unbind "<< mLocalAddress.toString();
}

bool CommsCarrierUDP::isStartedImp() const
{
	return (QAbstractSocket::BoundState == mUDPSocket.state());
}


qint64 CommsCarrierUDP::writeDataImp(const QByteArray &datagram, const NetworkAddress &address)
{
	return mUDPSocket.writeDatagram(datagram, address.ip(), address.port());
}

qint64 CommsCarrierUDP::readDataImp(char *data, qint64 maxlen, QHostAddress *host, quint16 *port)
{
	return mUDPSocket.readDatagram(data, maxlen, host, port);
}

bool CommsCarrierUDP::hasPendingDataImp()
{
	return false;
}

qint64 CommsCarrierUDP::pendingDataSizeImp()
{
	return 0;
}


QString CommsCarrierUDP::errorStringImp()
{
	return mUDPSocket.errorString();
}



NetworkAddress CommsCarrierUDP::addressImp()
{
	return mLocalAddress;
}


quint64 CommsCarrierUDP::minimalPacketIntervalImp()
{
	return OCTOMY_UDP_MINIMAL_PACKET_RATE;
}

quint64	CommsCarrierUDP::maximalPacketIntervalImp()
{
	return OCTOMY_UDP_MAXIMAL_PACKET_RATE;
}


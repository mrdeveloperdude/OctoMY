#include "CommsCarrierLocal.hpp"

#include "uptime/MethodGate.hpp"


// Local socket/server is very forgiving, we don't really observe any limitations.
#define OCTOMY_LOCAL_MAXIMAL_PACKET_RATE (9999999999)
#define OCTOMY_LOCAL_MINIMAL_PACKET_RATE (1)


// NOTE: We use 512 as the maximum practical UDP size for ipv4 over the internet
//       See this for discussion: http://stackoverflow.com/questions/1098897/what-is-the-largest-safe-udp-packet-size-on-the-internet
const qint32 CommsCarrierLocal::MAX_PAYLOAD_SIZE = 512;

CommsCarrierLocal::CommsCarrierLocal(QObject *parent)
	: CommsCarrier(parent)

{
	OC_METHODGATE();
	setObjectName("CommsCarrierLocal");

	connect(&mLocalServer, &QLocalServer::newConnection, this, [this]() {
		mLocalSocket = mLocalServer.nextPendingConnection();
		connect(mLocalSocket, &QLocalSocket::readyRead, this, &CommsCarrierLocal::carrierReadyRead);
		connect(mLocalSocket, &QLocalSocket::errorOccurred, this, [this]() {
			emit carrierError(mLocalSocket->errorString());
		});
	});
}



CommsCarrierLocal::~CommsCarrierLocal()
{
	OC_METHODGATE();
	if (mLocalSocket) {
		mLocalSocket->disconnectFromServer();
		delete mLocalSocket;
	}
	mLocalServer.close();
}


// CommsCarrier internal interface methods
//////////////////////////////////////////////////


void CommsCarrierLocal::configureImp()
{
	OC_METHODGATE();
}


bool CommsCarrierLocal::activateImp(const bool on)
{
	OC_METHODGATE();
	if (on) {
		if (!mLocalServer.listen(mServerName)) {
			qWarning() << "Failed to start local server:" << mLocalServer.errorString();
			return false;
		}
		qDebug() << "Local server started on" << mServerName;
	} else {
		mLocalServer.close();
		if (mLocalSocket) {
			mLocalSocket->disconnectFromServer();
			delete mLocalSocket;
			mLocalSocket = nullptr;
		}
		qDebug() << "Local server stopped";
	}
	return true;
}

void CommsCarrierLocal::setAddressImp(QSharedPointer<CarrierAddress> address)
{
	OC_METHODGATE();
	mLocalAddress = qSharedPointerDynamicCast<CarrierAddressLocal>(address);
}

bool CommsCarrierLocal::isActiveImp() const
{
	OC_METHODGATE();
	return mLocalServer.isListening();
}

qint64 CommsCarrierLocal::writeDataImp(const QByteArray &datagram, QSharedPointer<CarrierAddress> toAddress)
{
	OC_METHODGATE();
	if (!mLocalSocket || mLocalSocket->state() != QLocalSocket::ConnectedState) {
		qWarning() << "No active connection to write data";
		return -1;
	}
	return mLocalSocket->write(datagram);
}

qint64 CommsCarrierLocal::readDataImp(char *data, qint64 maxlen, QHostAddress *host, quint16 *port)
{
	OC_METHODGATE();
	Q_UNUSED(host);
	Q_UNUSED(port);
	if (!mLocalSocket || mLocalSocket->bytesAvailable() == 0) {
		return -1;
	}
	return mLocalSocket->read(data, maxlen);
}

bool CommsCarrierLocal::hasPendingDataImp()
{
	OC_METHODGATE();
	return mLocalSocket && mLocalSocket->bytesAvailable() > 0;
}

qint64 CommsCarrierLocal::pendingDataSizeImp()
{
	OC_METHODGATE();
	return mLocalSocket ? mLocalSocket->bytesAvailable() : 0;
}

QString CommsCarrierLocal::errorStringImp()
{
	OC_METHODGATE();
	return mLocalSocket ? mLocalSocket->errorString() : QString();
}

QSharedPointer<CarrierAddress> CommsCarrierLocal::addressImp()
{
	OC_METHODGATE();
	//return CarrierAddress(mServerName, 0);
	return mLocalAddress;
}

quint64 CommsCarrierLocal::minimalPacketIntervalImp()
{
	OC_METHODGATE();
	return OCTOMY_LOCAL_MINIMAL_PACKET_RATE;
}

quint64	CommsCarrierLocal::maximalPacketIntervalImp()
{
	OC_METHODGATE();
	return OCTOMY_LOCAL_MAXIMAL_PACKET_RATE;
}

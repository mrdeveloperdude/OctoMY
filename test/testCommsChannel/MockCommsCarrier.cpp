#include "MockCommsCarrier.hpp"

MockCommsCarrier::MockCommsCarrier(QObject *parent)
	: CommsCarrier(parent)
{

}


MockCommsCarrier::~MockCommsCarrier()
{

}

void MockCommsCarrier::writeMock(QByteArray data)
{

}

QByteArray MockCommsCarrier::readMock()
{
 return QByteArray();
}

bool MockCommsCarrier::startImp(NetworkAddress address)
{
	return false;
}


void MockCommsCarrier::stopImp()
{

}


bool MockCommsCarrier::isStartedImp() const
{
	return false;
}


qint64 MockCommsCarrier::writeDataImp(const QByteArray &datagram, const NetworkAddress &address)
{
	return 0;
}

qint64 MockCommsCarrier::readDataImp(char *data, qint64 maxlen, QHostAddress *host, quint16 *port)
{
	return 0;
}


bool MockCommsCarrier::hasPendingDataImp()
{
	return false;
}

qint64 MockCommsCarrier::pendingDataSizeImp()
{
	return 0;
}


QString MockCommsCarrier::errorStringImp()
{
	return "";
}


NetworkAddress MockCommsCarrier::addressImp()
{
	NetworkAddress  na;
	return na;
}



quint64 MockCommsCarrier::minimalPacketIntervalImp()
{
	return 0;
}

quint64	MockCommsCarrier::maximalPacketIntervalImp()
{
	return 0;
}

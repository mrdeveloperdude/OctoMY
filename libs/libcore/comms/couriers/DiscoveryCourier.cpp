#include "DiscoveryCourier.hpp"

#include "basic/NodeAssociate.hpp"
#include "comms/messages/MessageType.hpp"

DiscoveryCourier::DiscoveryCourier(QSharedPointer<NodeAssociate> ass, QObject *parent)
	: Courier("Discovery", Courier::FIRST_USER_ID+1, parent)
	, mAss(ass)
{
	qDebug()<<"CREATED DiscoveryCourier with PART="<<mAss->toString();
}

DiscoveryCourier::~DiscoveryCourier()
{
	qDebug()<<"DELETED DiscoveryCourier with PART="<<mAss->toString();
}

//Let the CommChannel know what we want
CourierMandate DiscoveryCourier::mandate()
{
	return CourierMandate(sizeof(quint32)+sizeof(quint16)+sizeof(quint64), 10, 1000, true, true);
}


//Hide adrresses by XORing them with a "random" number to counter bad
//gateway firmware
static const quint16 PORT_XOR=0x13e7;
static const quint32 IP_XOR=0x13e7268a;
static const quint64 BT_XOR=0x13e7268a13e7268a;

//Override to act on sending opportunity.
//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
quint16 DiscoveryCourier::sendingOpportunity(QDataStream &ds)
{
	qDebug()<<"Sending opportunity for "<<mName;
	quint16 bytes=0;
	NetworkAddress &nadr=mAss->publicAddress();
	QHostAddress pubAddr=nadr.ip();
	const quint32 ip4PubAddr=pubAddr.toIPv4Address();
	const quint16 port= mAss->publicAddress().port();
	const quint64 btAddr=mAss->bluetoothAddress().toUInt64();
	const quint32 ip4PubAddrXOR=ip4PubAddr ^ IP_XOR;
	const quint16 portXOR=port ^ PORT_XOR;
	const quint64 btAddrXOR=btAddr ^ BT_XOR;
	ds << ip4PubAddrXOR;
	bytes += sizeof(quint32);
	ds << portXOR;
	bytes += sizeof(quint16);
	ds << btAddrXOR;
	bytes += sizeof(quint64);
	qDebug()<<"TX "<<bytes<<" = ("<<ip4PubAddr<<":"<<port<<", bt="<<btAddr<<") = XOR("<<ip4PubAddrXOR<<":"<<portXOR<<", bt="<<btAddrXOR<<")";
	return bytes;
}



quint16 DiscoveryCourier::dataReceived(QDataStream &ds, quint16 availableBytes)
{
	quint16 ps=mandate().payloadSize;
	if(availableBytes!=ps) {
		qWarning()<<"ERROR: paayload size "<<ps<<" did not match received amount of data "<<availableBytes;
		return 0;
	}
	quint16 bytes=0;
	quint32 ip4RemoteAddr;
	quint16 remotePort;
	quint64 remoteBluetoothAddress;
	ds >> ip4RemoteAddr;
	ip4RemoteAddr ^= IP_XOR;
	bytes += sizeof(quint32);
	ds >> remotePort;
	bytes += sizeof(quint16);
	ds >> remoteBluetoothAddress;
	remoteBluetoothAddress ^= BT_XOR;
	bytes += sizeof(quint64);
	QHostAddress remoteAddr(ip4RemoteAddr);
	qDebug()<<"RX "<<bytes<<"="<<remoteAddr.toString()<<":"<<remotePort<<", bt="<<remoteBluetoothAddress;

	return bytes;
}

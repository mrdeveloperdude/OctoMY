#include "DiscoveryCourier.hpp"

#include "comms/discovery/DiscoveryParticipant.hpp"
#include "comms/messages/MessageType.hpp"

DiscoveryCourier::DiscoveryCourier(DiscoveryParticipant &part, QObject *parent)
	: Courier("Discovery", Courier::FIRST_USER_ID+2, parent)
	, part(part)
{

}



//Let the CommChannel know what we want
CourierMandate DiscoveryCourier::mandate()
{
	return CourierMandate(sizeof(quint32)+sizeof(quint16)+sizeof(quint64), 10, 1000, true, true);
}


//Hide adrresses by XORing them with a "random" number to counter bad
//gateway firmware
static const quint32 IP_XOR=0x13e7268a;
static const quint64 BT_XOR=0x13e7268a13e7268a;

//Override to act on sending opportunity.
//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
quint16 DiscoveryCourier::sendingOpportunity(QDataStream &ds)
{
	quint16 bytes=0;

	QHostAddress pubAddr=part.associate().publicAddress().ip;
	const quint32 ip4PubAddr=pubAddr.toIPv4Address() ^ IP_XOR;
	ds << ip4PubAddr;
	bytes += sizeof(quint32);
	ds << part.associate().publicAddress().port;
	bytes += sizeof(quint16);
	ds << (part.associate().bluetoothAddress().toUInt64() ^ BT_XOR);
	bytes += sizeof(quint64);
	qDebug()<<"TX "<<bytes<<"="<<pubAddr.toString()<<", bt="<<part.associate().bluetoothAddress().toUInt64();
	return 0;
}



quint16 DiscoveryCourier::dataReceived(QDataStream &ds, quint16 availableBytes)
{
	quint16 ps=mandate().payloadSize;
	if(availableBytes!=ps){
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

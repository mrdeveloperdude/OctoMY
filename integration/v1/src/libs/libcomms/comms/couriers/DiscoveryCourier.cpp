#include "DiscoveryCourier.hpp"

#include "basic/Associate.hpp"
#include "comms/messages/MessageType.hpp"
#include "basic/AddressEntry.hpp"
#include "utility/Utility.hpp"

#include <QSharedPointer>
#include <QString>

const QByteArray DiscoveryCourier::sLooperBuf=QString("lsdfjghsdklfjghklsdfjklsdjfklsdjflksjdflksjdf").toUtf8();
const LoopingBuffer DiscoveryCourier::sLooper(DiscoveryCourier::sLooperBuf);


DiscoveryCourier::DiscoveryCourier(QSharedPointer<Associate> ass, CommsChannel &comms, QObject *parent)
	: Courier("Discovery", Courier::FIRST_USER_ID+1, comms, parent)
	, mAss(ass)
	, mLastSend(0)
{
	qDebug()<<"CREATED DiscoveryCourier with PART="<<mAss->toString();
}

DiscoveryCourier::~DiscoveryCourier()
{
	qDebug()<<"DELETED DiscoveryCourier with PART="<<mAss->toString();
}

//Let the CommChannel know what we want
CourierMandate DiscoveryCourier::mandate() const
{
	return CourierMandate(sizeof(quint32)+sizeof(quint16)+sizeof(quint64), 10, mLastSend+1000, true, true);
}



//Override to act on sending opportunity.
//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
quint16 DiscoveryCourier::sendingOpportunity(QDataStream &ds)
{
	qDebug()<<"Sending opportunity for "<<name();
	NetworkAddress nAddr;
	const quint64 btAddr=mAss->bluetoothAddress().toUInt64();
	QByteArray ba;
	{
		QDataStream xds(ba);
		QSharedPointer<AddressEntry> ae=mAss->addressList().highestScore();
		if(!ae.isNull()) {
			nAddr=ae->address;
		}
		xds<<nAddr;
		xds<<btAddr;
	}
	//Hide adrresses by XORing them with a "random" number to counter bad gateway firmware
	sLooper.doXor(ba);
	ds << ba;
	const auto bytes=ba.size();
	mLastSend=utility::currentMsecsSinceEpoch<quint64>();
	qDebug().noquote().nospace()<<" **DISCOVERY** TX bytes="
	<< QString::number(bytes)
	<< " ( n="
	<< nAddr.toString()
	<< ", bt="
	<< btAddr
	<< ")";
	return static_cast<quint16>(bytes);


}



quint16 DiscoveryCourier::dataReceived(QDataStream &ds, quint16 availableBytes)
{
	quint16 ps=mandate().payloadSize;
	if(availableBytes!=ps) {
		qWarning()<<"ERROR: payload size "<<ps<<" did not match received amount of data "<<availableBytes;
		return 0;
	}
	NetworkAddress nAddr;
	quint64 btAddrInt=0;
	QByteArray ba;
	ds >> ba;
	auto bytes=ba.size();
	//Hide adrresses by XORing them with a "random" number to counter bad gateway firmware
	sLooper.doXor(ba);
	{
		QDataStream xds(ba);
		xds>>nAddr;
		xds>>btAddrInt;
	}
	const QBluetoothAddress btAddr(btAddrInt);
	qDebug() << " **DISCOVERY** RX bytes=" << bytes << " ( n=" << nAddr.toString() << ", bt=" << btAddr << ")";
	return static_cast<quint16>(bytes);
}

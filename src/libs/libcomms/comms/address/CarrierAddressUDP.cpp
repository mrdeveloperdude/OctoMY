#include "CarrierAddressUDP.hpp"

#include "uptime/MethodGate.hpp"
#include "CarrierAddressType.hpp"

#include <QDataStream>


CarrierAddressUDP::CarrierAddressUDP()
	: CarrierAddress(CarrierAddressType::UDP)
	//	, mIp("0.0.0.0")
	, mPort(0)
{
	OC_METHODGATE();
	//qDebug()<<"CREATING CarrierAddressUDP by default results in ip="<<mIP<<", port="<<mPort;
	
}

CarrierAddressUDP::CarrierAddressUDP(const CarrierAddressUDP &other)
	: CarrierAddress(CarrierAddressType::UDP)
	, mIP(other.mIP)
	, mPort(other.mPort)
{
	OC_METHODGATE();
	//qDebug()<<"CREATING CarrierAddressUDP from other(copy) results in ip="<<mIP<<", port="<<mPort;
}


CarrierAddressUDP::CarrierAddressUDP(QHostAddress ip, quint16 port)
	: CarrierAddress(CarrierAddressType::UDP)
	, mIP(ip)
	, mPort(port)
{
	OC_METHODGATE();
	//qDebug()<<"CREATING CarrierAddressUDP directly from ip="<<ip<<", port="<<port;
}


CarrierAddressUDP::CarrierAddressUDP(QVariantMap map)
	: CarrierAddress(CarrierAddressType::UDP)
	, mIP(map["ip"].toString())
	, mPort(static_cast<quint16>(map["port"].toInt()))
{
	OC_METHODGATE();
	//qDebug()<<"CREATING CarrierAddressUDP from map: "<<map<<" results in ip="<<mIP<<", port="<<mPort;
}


CarrierAddressUDP::~CarrierAddressUDP()
{
	OC_METHODGATE();
	//qDebug()<<"DELETING CarrierAddressUDP with ip="<<mIP<<", port="<<mPort;
}



/////////////////


//static quint64 gipct=0;
QHostAddress CarrierAddressUDP::ip() const
{
	OC_METHODGATE();
	/*
	gipct++;
	qDebug()<<"GET IP "<<gipct;
	qDebug()<<mIP<<mIP.isNull()<<mIP.isLoopback()<<mIP.isMulticast()<<mIP.protocol();
	QHostAddress arne;
	qDebug()<<arne<<arne.isNull()<<arne.isLoopback()<<arne.isMulticast()<<arne.protocol();
	arne=mIP;
	qDebug()<<arne<<arne.isNull()<<arne.isLoopback()<<arne.isMulticast()<<arne.protocol();

	return arne;
	*/
	return mIP;
}

quint16 CarrierAddressUDP::port() const
{
	OC_METHODGATE();
	return mPort;
}


void CarrierAddressUDP::setPort(quint16 port)
{
	OC_METHODGATE();
	mPort=port;
}

void CarrierAddressUDP::setIP(QHostAddress ip)
{
	OC_METHODGATE();
	mIP=ip;
}




bool CarrierAddressUDP::isValidIP(bool allowLoopback, bool allowMulticast, bool allowIPv6) const
{
	OC_METHODGATE();
	if((static_cast<quint16>(0)) == mPort) {
		return false;
	}
	if(mIP.isNull()) {
		return false;
	}
	if(!allowLoopback && mIP.isLoopback()) {
		return false;
	}
	if(!allowMulticast && mIP.isMulticast()) {
		return false;
	}
	if(!allowIPv6 && QAbstractSocket::IPv4Protocol != mIP.protocol()) {
		return false;
	}
	return true;
}




bool CarrierAddressUDP::isIPv4() const
{
	OC_METHODGATE();
	return QAbstractSocket::IPv4Protocol==mIP.protocol();
}

bool CarrierAddressUDP::isIPv6() const
{
	OC_METHODGATE();
	return QAbstractSocket::IPv6Protocol==mIP.protocol();
}


/////////////////



bool CarrierAddressUDP::isValid() const
{
	OC_METHODGATE();
	return isValidIP();
}


bool CarrierAddressUDP::equal(const CarrierAddress &o) const
{
	OC_METHODGATE();
	if(o.type() != mType){
		return false;
	}
	auto udp_o = static_cast<const CarrierAddressUDP &>(o);
	return udp_o.mPort==mPort && udp_o.mIP==mIP;
}



void CarrierAddressUDP::toVariantMap(QVariantMap &map) const
{
	OC_METHODGATE();
	map["type"] = CarrierAddressTypeToString(mType);
	map["ip"] = mIP.toString();
	map["port"] = mPort;
}



QVariantMap CarrierAddressUDP::toVariantMap() const
{
	OC_METHODGATE();
	QVariantMap map;
	toVariantMap(map);
	return map;
}



QString CarrierAddressUDP::toString() const
{
	OC_METHODGATE();
	return mIP.toString()+":"+QString::number(mPort);
}


// TODO: Add support for IPv6
void CarrierAddressUDP::fromString(QString str, bool allowOnlyAddress)
{
	OC_METHODGATE();
	QStringList list = str.split(":");
	const auto sz=list.size();
	
	if(sz<1 || sz>2 || ( !allowOnlyAddress && sz < 2 ) ) {
		// Indicate error if number of parts is out of range
		mIP=QHostAddress();
		mPort=0;
	} else {
		// Always take address
		mIP=QHostAddress(list[0]);
		// Take port if it is there
		if(2==sz) {
			mPort=static_cast<quint16>(list[1].toInt());
		}
	}
}


void CarrierAddressUDP::toStream(QDataStream &ds) const
{
	OC_METHODGATE();
	ds<<mPort;
	ds<<mIP;
}


void CarrierAddressUDP::fromStream(QDataStream &ds)
{
	OC_METHODGATE();
	ds>>mPort;
	ds>>mIP;
}

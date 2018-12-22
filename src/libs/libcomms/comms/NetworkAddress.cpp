#include "NetworkAddress.hpp"

#include "utility/Standard.hpp"

#include <QDataStream>

NetworkAddress::NetworkAddress()
//	: mIp("0.0.0.0")
	: mPort(0)
{
	OC_METHODGATE();
	//qDebug()<<"CREATING NetworkAddress by default results in ip="<<mIP<<", port="<<mPort;
}


NetworkAddress::NetworkAddress(const NetworkAddress &other)
	: mIP(other.mIP)
	, mPort(other.mPort)
{
	OC_METHODGATE();
	//qDebug()<<"CREATING NetworkAddress from other(copy) results in ip="<<mIP<<", port="<<mPort;
}


NetworkAddress::NetworkAddress(QHostAddress ip, quint16 port)
	: mIP(ip)
	, mPort(port)
{
	OC_METHODGATE();
	//qDebug()<<"CREATING NetworkAddress directly from ip="<<ip<<", port="<<port;
}


NetworkAddress::NetworkAddress(QVariantMap map)
	: mIP(map["ip"].toString())
	, mPort(static_cast<quint16>(map["port"].toInt()))
{
	OC_METHODGATE();
	//qDebug()<<"CREATING NetworkAddress from map: "<<map<<" results in ip="<<mIP<<", port="<<mPort;
}


NetworkAddress::~NetworkAddress()
{
	OC_METHODGATE();
	//qDebug()<<"DELETING NetworkAddress with ip="<<mIP<<", port="<<mPort;
}





//static quint64 gipct=0;
QHostAddress NetworkAddress::ip() const
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

quint16 NetworkAddress::port() const
{
	OC_METHODGATE();
	return mPort;
}


void NetworkAddress::setPort(quint16 port)
{
	OC_METHODGATE();
	mPort=port;
}

void NetworkAddress::setIP(QHostAddress ip)
{
	OC_METHODGATE();
	mIP=ip;
}

QVariantMap NetworkAddress::toVariantMap() const
{
	OC_METHODGATE();
	QVariantMap map;
	map["ip"]=mIP.toString();
	map["port"]=mPort;
	return map;
}

QString NetworkAddress::toString() const
{
	OC_METHODGATE();
	return mIP.toString()+":"+QString::number(mPort);
}


void NetworkAddress::fromString(QString str, bool allowOnlyAddress)
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

bool NetworkAddress::isValid(bool allowLoopback, bool allowMulticast, bool allowIPv6) const
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


bool NetworkAddress::isIPv4() const
{
	return QAbstractSocket::IPv4Protocol==mIP.protocol();
}

bool NetworkAddress::isIPv6() const
{
	return QAbstractSocket::IPv6Protocol==mIP.protocol();
}


bool NetworkAddress::operator==(const NetworkAddress &o) const
{
	OC_METHODGATE();
	return o.mPort==mPort && o.mIP==mIP;
}

bool NetworkAddress::operator!=(const NetworkAddress &o) const
{
	OC_METHODGATE();
	return !(o == *this);
}


void NetworkAddress::toStream(QDataStream &ds) const
{
	ds<<mPort;
	ds<<mIP;
}


void NetworkAddress::fromStream(QDataStream &ds)
{
	ds>>mPort;
	ds>>mIP;
}

const QDebug &operator<<(QDebug &d, const NetworkAddress &na)
{
	d.nospace() << na.toString();
	return d.maybeSpace();
}


QDataStream &operator<<(QDataStream &ds, const NetworkAddress &addr)
{
	addr.toStream(ds);
	return ds;
}

QDataStream &operator>>(QDataStream &ds, NetworkAddress &addr)
{
	addr.fromStream(ds);
	return ds;
}

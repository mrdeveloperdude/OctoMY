#include "NetworkAddress.hpp"

#include "basic/Standard.hpp"


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
	, mPort(map["port"].toInt())
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
QHostAddress NetworkAddress::ip()
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

quint16 NetworkAddress::port()
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

QVariantMap NetworkAddress::toVariantMap()
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

bool NetworkAddress::isValid(bool allowLoopback, bool allowMulticast)
{
	OC_METHODGATE();
	return ( (((quint16)0) != mPort) && (!mIP.isNull()) && (allowLoopback || !mIP.isLoopback()) && (allowMulticast || !mIP.isMulticast()));
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





const QDebug &operator<<(QDebug &d, const NetworkAddress &na){
	d.nospace() << na.toString();
	return d.maybeSpace();
}

#include "ClientSignature.hpp"

ClientSignature::ClientSignature()
{

}


ClientSignature::ClientSignature(const ClientSignature &other)
	: mShortHandID(other.mShortHandID)
	, mAddress(other.mAddress)
{

}

ClientSignature::ClientSignature(const quint64 &shortHandID, const NetworkAddress &address)
	: mShortHandID(shortHandID)
	, mAddress(address)
{
}



ClientSignature::ClientSignature(const QString &fullID, const NetworkAddress &address)
	: mShortHandID(fullID.mid(0,16).toULongLong(nullptr,16))
	, mFullID(fullID)
	, mAddress(address)
{
}



ClientSignature::ClientSignature(const QString &fullID)
	: mShortHandID(fullID.mid(0,16).toULongLong(nullptr,16))
{
}


const QString ClientSignature::toString() const
{
	return QString::number(mShortHandID,16)+"-"+mAddress.toString();
}


quint64 ClientSignature::shortHandID() const
{
	return mShortHandID;
}

QString ClientSignature::fullID() const
{
	return mFullID;
}


NetworkAddress ClientSignature::address() const
{
	return mAddress;
}


void ClientSignature::setAddress(const NetworkAddress &address)
{
	mAddress=address;
}


void ClientSignature::setShortHandID(const quint64 &id)
{
	mShortHandID=id;
}



void ClientSignature::setFullID(const QString &fullID)
{
	mShortHandID=(fullID.mid(0,16).toULongLong(nullptr,16));
	mFullID=fullID;
}


bool ClientSignature::isValid() const
{
	return mAddress.isValid(true,false);
}



const QDebug &operator<<(QDebug &d, const ClientSignature &sig)
{
	d.nospace() << "ClientSignature["<<sig.toString()<<"("<<sig.fullID()<<")]";
	return d.maybeSpace();
}

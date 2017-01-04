#include "CommsSignature.hpp"

CommsSignature::CommsSignature()
{

}


CommsSignature::CommsSignature(const CommsSignature &other)
	: mShortHandID(other.mShortHandID)
	, mAddress(other.mAddress)
{

}

CommsSignature::CommsSignature(const quint64 &shortHandID, const NetworkAddress &address)
	: mShortHandID(shortHandID)
	, mAddress(address)
{
}



CommsSignature::CommsSignature(const QString &fullID, const NetworkAddress &address)
	: mShortHandID(fullID.mid(0,16).toULongLong(nullptr,16))
	, mFullID(fullID)
	, mAddress(address)
{
}



CommsSignature::CommsSignature(const QString &fullID)
	: mShortHandID(fullID.mid(0,16).toULongLong(nullptr,16))
{
}


const QString CommsSignature::toString() const
{
	return QString::number(mShortHandID,16)+"-"+mAddress.toString();
}


quint64 CommsSignature::shortHandID() const
{
	return mShortHandID;
}

QString CommsSignature::fullID() const
{
	return mFullID;
}


NetworkAddress CommsSignature::address() const
{
	return mAddress;
}


void CommsSignature::setAddress(const NetworkAddress &address)
{
	mAddress=address;
}


void CommsSignature::setShortHandID(const quint64 &id)
{
	mShortHandID=id;
}



void CommsSignature::setFullID(const QString &fullID)
{
	mShortHandID=(fullID.mid(0,16).toULongLong(nullptr,16));
	mFullID=fullID;
}


bool CommsSignature::isValid() const
{
	return mAddress.isValid(true,false);
}



const QDebug &operator<<(QDebug &d, const CommsSignature &sig)
{
	d.nospace() << "ClientSignature["<<sig.toString()<<"("<<sig.fullID()<<"), valid="<<sig.isValid()<<"]";
	return d.maybeSpace();
}

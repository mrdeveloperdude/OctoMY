#ifndef SYNCPARAMETER_HPP
#define SYNCPARAMETER_HPP

#include "../libutil/utility/Standard.hpp"

#include <QtGlobal>

#include "ISyncParameter.hpp"

class SyncContext;




template <typename T>
class SyncParameter: public ISyncParameter
{
private:

	SyncContext &mCTX;
	T mLocalValue;
	T mRemoteValue;
	quint64 mLocalTimestamp;
	quint64 mRemoteTimestamp;

	bool mLocalDirty;


public:

	SyncParameter(const T &val, SyncContext &ctx);
	void setLocalValue(const T &val);
	void setLocalNoChange();
	void setRemoteValue(const T &val);
	void setRemoteNoChange();


	T localValue();
	T remoteValue();
	quint64 localTimestamp();
	quint64 remoteTimestamp();
	T bestValue(bool isLocal) const;


	//Not so sure what I can use this for:
	/*
	bool isRemoteConfirmed() const;
	bool isLocalConfirmed() const;
	bool isInSync() const;
	*/


	// ISyncParameter interface
public:
	quint16 bytes() const Q_DECL_OVERRIDE;
	QString toString() const Q_DECL_OVERRIDE;
	void flush() Q_DECL_OVERRIDE;
	QDataStream &send(QDataStream &ds) Q_DECL_OVERRIDE;
	QDataStream &receive(QDataStream &ds) Q_DECL_OVERRIDE;

	bool hasPendingSync() const Q_DECL_OVERRIDE;
	bool hasPendingAck() const Q_DECL_OVERRIDE;
	void ack() Q_DECL_OVERRIDE;

	QDebug &toDebug(QDebug &d) const Q_DECL_OVERRIDE;


};


/*




*/
////////////////////////////////////////////////////////////////////////////////

#include "SyncContext.hpp"

template <typename T>
SyncParameter<T>::SyncParameter(const T &val, SyncContext &ctx)
	: mCTX(ctx)
	, mLocalValue(val)
	, mRemoteValue(val)
	, mLocalTimestamp(mCTX.now())
	, mRemoteTimestamp(0)
	, mLocalDirty(false)
{
}

template <typename T>
void SyncParameter<T>::setLocalValue(const T &val)
{
	mLocalValue=val;
	mLocalTimestamp=mCTX.now();
	mLocalDirty=true;
}

template <typename T>
void SyncParameter<T>::setLocalNoChange()
{
	mLocalTimestamp=mCTX.now();
}


template <typename T>
void SyncParameter<T>::setRemoteValue(const T &val)
{
	mRemoteValue=val;
	mRemoteTimestamp=mCTX.now();
}


template <typename T>
void SyncParameter<T>::setRemoteNoChange()
{
	mRemoteTimestamp=mCTX.now();
}



template <typename T>
T SyncParameter<T>::localValue()
{
	return mLocalValue;
}

template <typename T>
T SyncParameter<T>::remoteValue()
{
	return mRemoteValue;
}

template <typename T>
quint64 SyncParameter<T>::localTimestamp()
{
	return mLocalTimestamp;
}

template <typename T>
quint64 SyncParameter<T>::remoteTimestamp()
{
	return mRemoteTimestamp;
}


template <typename T>
T SyncParameter<T>::bestValue(bool isLocal) const
{
	if(mLocalTimestamp==mRemoteTimestamp) {
		return isLocal?mLocalValue:mRemoteValue;
	} else {
		return (mLocalTimestamp>mRemoteTimestamp)?mLocalValue:mRemoteValue;
	}
}

/*
template <typename T>
bool SyncParameter<T>::isRemoteConfirmed() const
{
	const quint64 rtt=mCTX.roundTripTime();
	const quint64 now=mCTX.now();
	return (now < (mRemoteTimestamp + rtt) );
}

template <typename T>
bool SyncParameter<T>::isLocalConfirmed() const
{
	const quint64 rtt=mCTX.roundTripTime();
	const quint64 now=mCTX.now();
	return (now < (mLocalTimestamp + rtt) );
}


template <typename T>
bool SyncParameter<T>::isInSync() const
{
	return (isRemoteConfirmed() && isLocalConfirmed());
}
*/


////////////////////////////////////////////////////////////////////////////////
/// helper to convert different types to string. Defaults to QString::number()
/// specialized for each type this does not work for

#include "../libcore/puppet/Pose.hpp"


#include <QGeoCoordinate>

template<typename T>
QString valueToString (const T v)
{
	return QString::number(v);
}


static QString valueToString (const Pose &v)
{
	return v.toString();
}

static QString valueToString (const QGeoCoordinate &v)
{
	return v.toString();
}

////////////////////////////////////////////////////////////////////////////////
// Implementation of ISyncParameter interface //////////////////////////////////


template <typename T>
quint16 SyncParameter<T>::bytes() const
{
	return sizeof(T);
}




template <typename T>
QString SyncParameter<T>::toString() const
{
	const QString localValueString=valueToString(mLocalValue);
	const QString remoteValueString=valueToString(mRemoteValue);
	return QString("SyncParameter[ localValue=")+localValueString+"("+QString::number(mLocalTimestamp)+QString("), remoteValue=")+remoteValueString+"("+QString::number(mRemoteTimestamp)+"), hasPendingSync="+(hasPendingSync()?"true":"false")+", hasPendingAck="+(hasPendingAck()?"true":"false")+"]";//, ctx(now="+QString::number(mCTX.now())+", rtt="+QString::number(mCTX.roundTripTime())+")]";
}



template <typename T>
QDebug &SyncParameter<T>::toDebug(QDebug &d) const
{
	const QString localValueString=valueToString(mLocalValue);
	const QString remoteValueString=valueToString(mRemoteValue);
	d.nospace() << "SyncParameter[ localValue=" << localValueString << "(" << mLocalTimestamp << "), remoteValue=" << remoteValueString << "(" << mRemoteTimestamp << "), hasPendingSync=" << (hasPendingSync()?"true":"false")  << ", hasPendingAck=" << (hasPendingAck()?"true":"false") <<"]";//, ctx(now=" << (mCTX.now()) << ", rtt=" << mCTX.roundTripTime() << ")]";
	return d.maybeSpace();
}


template <typename T>
void SyncParameter<T>::flush()
{
	mLocalDirty=true;
}

template <typename T>
QDataStream &SyncParameter<T>::send(QDataStream &ds)
{
	//qDebug()<<"TX "<<valueToString(bestValue(false));
	ds << bestValue(false);
	//setLocalNoChange(); DONT DO THIS HERE, LET IT HAPPEN UPON RECEIVAL OF ACK
	return ds;
}

template <typename T>
QDataStream &SyncParameter<T>::receive(QDataStream &ds)
{
	T value;//=(T)0;
	ds >> value;
	//qDebug()<<"RX "<<valueToString(value);
	setRemoteValue(value);
	return ds;
}

template <typename T>
bool SyncParameter<T>::hasPendingSync() const
{
	return mLocalDirty;
}

template <typename T>
bool SyncParameter<T>::hasPendingAck() const
{
	return ( (mLocalTimestamp > mRemoteTimestamp) && (mLocalTimestamp < (mCTX.now() - mCTX.roundTripTime()) ));
}

template <typename T>
void SyncParameter<T>::ack()
{
	if(hasPendingAck()) {
		mLocalDirty=false;
	} else {
		qWarning()<<"ERROR: received ack when not expecting it";
	}
}

#endif // SYNCPARAMETER_HPP

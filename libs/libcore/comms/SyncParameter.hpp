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


public:

	SyncParameter(const T &val, SyncContext &ctx);
	void setLocalValue(const T &val);
	void setRemoteNoChange();
	void setLocalNoChange();
	void setRemoteValue(const T &val);
	T bestValue() const;
	bool isRemoteConfirmed() const;
	bool isLocalConfirmed() const;
	bool isInSync() const;
	T localValue();
	T remoteValue();
	quint64 localTimestamp();
	quint64 remoteTimestamp();

	// ISyncParameter interface
public:
	quint16 bytes() const Q_DECL_OVERRIDE;
	QString toString() const Q_DECL_OVERRIDE;
	QDataStream &operator<<(QDataStream &ds) Q_DECL_OVERRIDE;
	QDataStream &operator>>(QDataStream &ds) Q_DECL_OVERRIDE;

	bool hasPendingSync() const Q_DECL_OVERRIDE;
	bool hasPendingAck() const Q_DECL_OVERRIDE;
	void ack() Q_DECL_OVERRIDE;


};



////////////////////////////////////////////////////////////////////////////////

#include "SyncContext.hpp"

template <typename T>
SyncParameter<T>::SyncParameter(const T &val, SyncContext &ctx)
	: mCTX(ctx)
	, mLocalValue(val)
	, mRemoteValue(val)
	, mLocalTimestamp(mCTX.now())
	, mRemoteTimestamp(0)
{
}

template <typename T>
void SyncParameter<T>::setLocalValue(const T &val)
{
	mLocalValue=val;
	mLocalTimestamp=mCTX.now();
}

template <typename T>
void SyncParameter<T>::setLocalNoChange()
{
	mLocalTimestamp=mCTX.now();
}

template <typename T>
void SyncParameter<T>::setRemoteNoChange()
{
	mRemoteTimestamp=mCTX.now();
}

template <typename T>
void SyncParameter<T>::setRemoteValue(const T &val)
{
	mRemoteValue=val;
	mRemoteTimestamp=mCTX.now();
}

template <typename T>
T SyncParameter<T>::bestValue() const
{
	//TODO: Handle case when both are equal better (by integrating isAgentSide or something like it )
	return mLocalTimestamp>mRemoteTimestamp?mLocalValue:mRemoteValue;
}

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


////////////////////////////////////////////////////////////////////////////////
/// helper to convert different types to string. Defaults to QString::number()
/// specialized for each type this does not work for

#include "../libcore/puppet/Pose.hpp"


#include <QGeoCoordinate>

template<typename T>
QString arne (const T v)
{
	return QString::number(v);
}


QString arne (const Pose &v)
{
	return v.toString();
}


QString arne (const QGeoCoordinate &v)
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
	const QString localValueString=arne(mLocalValue);
	const QString remoteValueString=arne(mRemoteValue);
	return QString("SyncParameter[ local=")+localValueString+"("+QString::number(mLocalTimestamp)+", sync="+(isLocalConfirmed()?"true":"false")+QString("), remote=")+remoteValueString+"("+QString::number(mRemoteTimestamp)+", sync="+(isRemoteConfirmed()?"true":"false")+"), sync="+(isInSync()?"true":"false")+", ctx(now="+QString::number(mCTX.now())+", rtt="+QString::number(mCTX.roundTripTime())+")]";
}

template <typename T>
QDataStream &SyncParameter<T>::operator<<(QDataStream &ds)
{
	ds << bestValue();
	//setLocalNoChange(); DONT DO THIS HERE, LET IT HAPPEN UPON RECEIVAL OF ACK
	return ds;
}

template <typename T>
QDataStream &SyncParameter<T>::operator>>(QDataStream &ds)
{
	T value;//=(T)0;
	ds >> value;
	setRemoteValue(value);
	return ds;
}

template <typename T>
bool SyncParameter<T>::hasPendingSync() const
{
	const bool localIsNewest=(mLocalTimestamp > mRemoteTimestamp);
	//const bool	&& (mLocalTimestamp < (mCTX.now() - mCTX.roundTripTime()) ));
	return false; //TODO: FIX ME
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

	} else {
		qWarning()<<"ERROR: received ack when not expecting it";
	}
}

#endif // SYNCPARAMETER_HPP

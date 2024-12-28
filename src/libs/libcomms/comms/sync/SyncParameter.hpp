#ifndef SYNCPARAMETER_HPP
#define SYNCPARAMETER_HPP

#include "uptime/MethodGate.hpp"
#include "utility/network/SerialSize.hpp"
#include "ISyncParameter.hpp"
#include "pose/Pose.hpp"
#include "comms/ardumyGeoCoordinate.hpp"

#include <QObject>
#include <QGeoCoordinate>


class SyncContext;


template <typename T>
class SyncParameter: public ISyncParameter
{

private:
	SyncContext &mCTX;
	QString mName;
	T mLocalValue;
	T mRemoteValue;
	quint64 mLocalTimestamp;
	quint64 mRemoteTimestamp;
	bool mNeedToSendAck;
	bool mNeedToSendDataAndReceiveAck;

public:
	SyncParameter(const QString &name, const T &val, SyncContext &ctx);
	void setLocalValue(const T &val);
	void setLocalNoChange();
	void setRemoteValue(const T &val);
	void setRemoteNoChange();

	void markLocalValueAsUpdated();
	void markRemoteValueAsUpdated();
	T &localValueRef();
	T &remoteValueRef();

	T localValue() const;
	T remoteValue() const;
	quint64 localTimestamp() const;
	quint64 remoteTimestamp() const;
	T bestValue(bool isLocal) const;

	// ISyncParameter interface
public:
	quint16 bytes() const override;
	QString toString() const override;
	QString name() const  override;
	void forceSync() override;
	QDataStream &send(QDataStream &ds) override;
	QDataStream &receive(QDataStream &ds) override;

	bool needToSendAck() const override;
	bool needToSendDataAndReceiveAck() const override;
	void ackSent() override;
	void ack() override;

	QDebug &toDebug(QDebug &d) const override;

};


////////////////////////////////////////////////////////////////////////////////

#include "SyncContext.hpp"

template <typename T>
SyncParameter<T>::SyncParameter(const QString &name, const T &val, SyncContext &ctx)
	: mCTX(ctx)
	, mName(name)
	, mLocalValue(val)
	, mRemoteValue(val)
	, mLocalTimestamp(mCTX.now())
	, mRemoteTimestamp(0)
	, mNeedToSendAck(false)
	, mNeedToSendDataAndReceiveAck(false)
{
	OC_METHODGATE();
}

template <typename T>
void SyncParameter<T>::setLocalValue(const T &val)
{
	OC_METHODGATE();
	mLocalValue=val;
	markLocalValueAsUpdated();
}

template <typename T>
void SyncParameter<T>::setLocalNoChange()
{
	OC_METHODGATE();
	markLocalValueAsUpdated();
}


template <typename T>
void SyncParameter<T>::setRemoteValue(const T &val)
{
	OC_METHODGATE();
	mRemoteValue=val;
	markRemoteValueAsUpdated();
}


template <typename T>
void SyncParameter<T>::setRemoteNoChange()
{
	OC_METHODGATE();
	markRemoteValueAsUpdated();
}


template <typename T>
void SyncParameter<T>::markLocalValueAsUpdated()
{
	OC_METHODGATE();
	mLocalTimestamp=mCTX.now();
	mNeedToSendDataAndReceiveAck=true;
	signalValueChanged();
}


template <typename T>
void SyncParameter<T>::markRemoteValueAsUpdated()
{
	OC_METHODGATE();
	mRemoteTimestamp=mCTX.now();
	mNeedToSendAck=true;
	signalValueChanged();
}


template <typename T>
T SyncParameter<T>::localValue() const
{
	OC_METHODGATE();
	return mLocalValue;
}

template <typename T>
T SyncParameter<T>::remoteValue() const
{
	OC_METHODGATE();
	return mRemoteValue;
}


template <typename T>
T &SyncParameter<T>::localValueRef()
{
	OC_METHODGATE();
	return mLocalValue;
}

template <typename T>
T &SyncParameter<T>::remoteValueRef()
{
	OC_METHODGATE();
	return mRemoteValue;
}

template <typename T>
quint64 SyncParameter<T>::localTimestamp() const
{
	OC_METHODGATE();
	return mLocalTimestamp;
}

template <typename T>
quint64 SyncParameter<T>::remoteTimestamp() const
{
	OC_METHODGATE();
	return mRemoteTimestamp;
}


template <typename T>
T SyncParameter<T>::bestValue(bool isLocal) const
{
	OC_METHODGATE();
	if(mLocalTimestamp==mRemoteTimestamp) {
		return isLocal?mLocalValue:mRemoteValue;
	} else {
		return (mLocalTimestamp>mRemoteTimestamp)?mLocalValue:mRemoteValue;
	}
}






////////////////////////////////////////////////////////////////////////////////
/// helper to convert different types to string. Defaults to QString::number()
/// specialized for each type this does not work for


template<typename T>
QString valueToString (const T v)
{
	OC_FUNCTIONGATE();
	return QString::number(v);
}


////////////////////////////////////////////////////////////////////////////////
// Implementation of ISyncParameter interface //////////////////////////////////


template <typename T>
quint16 SyncParameter<T>::bytes() const
{
	OC_METHODGATE();
	SerialSize size;
	T t=localValue();
	quint64 s=size(t);
	return static_cast<quint16>(s);
}




template <typename T>
QString SyncParameter<T>::toString() const
{
	OC_METHODGATE();
	const QString localValueString=ardumyActuatorValueToString(mLocalValue);
	const QString remoteValueString=ardumyActuatorValueToString(mRemoteValue);
	return QString("SyncParameter ")+mName+" [ localValue="+localValueString+" ("+QString::number(mLocalTimestamp)+QString("), remoteValue=")+remoteValueString+" ("+QString::number(mRemoteTimestamp)+"), needToSendAck="+(needToSendAck()?"true":"false")+", needToSendDataAndReceiveAck="+(needToSendDataAndReceiveAck()?"true":"false")+"]";//, ctx(now="+QString::number(mCTX.now())+", rtt="+QString::number(mCTX.roundTripTime())+")]";
}


template <typename T>
QString SyncParameter<T>::name() const
{
	OC_METHODGATE();
	return mName;
}



template <typename T>
QDebug &SyncParameter<T>::toDebug(QDebug &d) const
{
	OC_METHODGATE();
	const QString localValueString=ardumyActuatorValueToString(mLocalValue);
	const QString remoteValueString=ardumyActuatorValueToString(mRemoteValue);
	d.nospace() << "SyncParameter "<< mName <<" [ localValue=" << localValueString << " (" << mLocalTimestamp << "), remoteValue=" << remoteValueString << " (" << mRemoteTimestamp << "), needToSendAck=" << (needToSendAck()?"true":"false")  << ", needToSendDataAndReceiveAck=" << (needToSendDataAndReceiveAck()?"true":"false") <<"]";//, ctx(now=" << (mCTX.now()) << ", rtt=" << mCTX.roundTripTime() << ")]";
	return d.maybeSpace();
}


template <typename T>
void SyncParameter<T>::forceSync()
{
	OC_METHODGATE();
	//qDebug()<<" --==## %% % FORCE SYNC";
	mNeedToSendAck=true;
	mNeedToSendDataAndReceiveAck=true;
}

template <typename T>
QDataStream &SyncParameter<T>::send(QDataStream &ds)
{
	OC_METHODGATE();
	//qDebug()<<"TX "<<valueToString(bestValue(false));
	ds << bestValue(false);
	return ds;
}

template <typename T>
QDataStream &SyncParameter<T>::receive(QDataStream &ds)
{
	OC_METHODGATE();
	T value;//=(T)0;
	ds >> value;
	//qDebug()<<"RX "<<valueToString(value);
	setRemoteValue(value);
	return ds;
}

template <typename T>
bool SyncParameter<T>::needToSendAck() const
{
	OC_METHODGATE();
	return mNeedToSendAck;
}

template <typename T>
bool SyncParameter<T>::needToSendDataAndReceiveAck() const
{
	OC_METHODGATE();
	return mNeedToSendDataAndReceiveAck;
}


template <typename T>
void SyncParameter<T>::ackSent()
{
	OC_METHODGATE();
	if(!mNeedToSendAck) {
		qWarning()<<"WARNING: sent ack when not expecting it";
	}
	mNeedToSendAck=false;
}


template <typename T>
void SyncParameter<T>::ack()
{
	OC_METHODGATE();
	if(!mNeedToSendDataAndReceiveAck) {
		qWarning()<<"WARNING: received ack when not expecting it";
	}
	mNeedToSendDataAndReceiveAck=false;
}

#endif
// SYNCPARAMETER_HPP

#ifndef SYNCPARAMETER_HPP
#define SYNCPARAMETER_HPP

#include "../libutil/utility/Standard.hpp"

#include <QtGlobal>

#include "ISyncParameter.hpp"

#include "../libutil/utility/SerialSize.hpp"

class SyncContext;

#include <QObject>


class SyncSignaler: public QObject
{
	Q_OBJECT
signals:
	void valuesChanged(ISyncParameter *) const;
};

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
	SyncSignaler mSignaler;

public:

	SyncParameter(const QString &name, const T &val, SyncContext &ctx);
	void setLocalValue(const T &val);
	void setLocalNoChange();
	void setRemoteValue(const T &val);
	void setRemoteNoChange();


	T localValue() const;
	T remoteValue() const;
	quint64 localTimestamp() const;
	quint64 remoteTimestamp() const;
	T bestValue(bool isLocal) const;

public:

	void signalValueChanged();
	void hookSignals(QObject &ob);
	void unHookSignals(QObject &ob);



	// ISyncParameter interface
public:
	quint16 bytes() const Q_DECL_OVERRIDE;
	QString toString() const Q_DECL_OVERRIDE;
	QString name() const  Q_DECL_OVERRIDE;
	void forceSync() Q_DECL_OVERRIDE;
	QDataStream &send(QDataStream &ds) Q_DECL_OVERRIDE;
	QDataStream &receive(QDataStream &ds) Q_DECL_OVERRIDE;

	bool needToSendAck() const Q_DECL_OVERRIDE;
	bool needToSendDataAndReceiveAck() const Q_DECL_OVERRIDE;
	void ackSent() Q_DECL_OVERRIDE;
	void ack() Q_DECL_OVERRIDE;

	QDebug &toDebug(QDebug &d) const Q_DECL_OVERRIDE;


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
}

template <typename T>
void SyncParameter<T>::setLocalValue(const T &val)
{
	mLocalValue=val;
	mLocalTimestamp=mCTX.now();
	mNeedToSendDataAndReceiveAck=true;
	signalValueChanged();
}

template <typename T>
void SyncParameter<T>::setLocalNoChange()
{
	setLocalValue(mLocalValue);
}


template <typename T>
void SyncParameter<T>::setRemoteValue(const T &val)
{
	mRemoteValue=val;
	mRemoteTimestamp=mCTX.now();
	mNeedToSendAck=true;
	signalValueChanged();
}


template <typename T>
void SyncParameter<T>::setRemoteNoChange()
{
	setRemoteValue(mRemoteValue);
}



template <typename T>
T SyncParameter<T>::localValue() const
{
	return mLocalValue;
}

template <typename T>
T SyncParameter<T>::remoteValue() const
{
	return mRemoteValue;
}

template <typename T>
quint64 SyncParameter<T>::localTimestamp() const
{
	return mLocalTimestamp;
}

template <typename T>
quint64 SyncParameter<T>::remoteTimestamp() const
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


template <typename T>
void SyncParameter<T>::signalValueChanged()
{
	emit mSignaler.valuesChanged(this);
}

template <typename T>
void SyncParameter<T>::hookSignals(QObject &ob)
{
	if(!QObject::connect(&mSignaler,SIGNAL(valuesChanged(ISyncParameter *)),&ob,SLOT(onValueChanged(ISyncParameter *)),OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect "<<ob.objectName();
	}
}


template <typename T>
void SyncParameter<T>::unHookSignals(QObject &ob)
{
	if(!QObject::disconnect(&mSignaler,SIGNAL(valuesChanged(ISyncParameter *)),&ob,SLOT(onValueChanged(ISyncParameter *)))) {
		qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
	}
}





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
	SerialSize size;
	quint64 s=size(localValue());
	return (quint16)s;
}




template <typename T>
QString SyncParameter<T>::toString() const
{
	const QString localValueString=valueToString(mLocalValue);
	const QString remoteValueString=valueToString(mRemoteValue);
	return QString("SyncParameter ")+mName+" [ localValue="+localValueString+" ("+QString::number(mLocalTimestamp)+QString("), remoteValue=")+remoteValueString+" ("+QString::number(mRemoteTimestamp)+"), needToSendAck="+(needToSendAck()?"true":"false")+", needToSendDataAndReceiveAck="+(needToSendDataAndReceiveAck()?"true":"false")+"]";//, ctx(now="+QString::number(mCTX.now())+", rtt="+QString::number(mCTX.roundTripTime())+")]";
}


template <typename T>
QString SyncParameter<T>::name() const
{
	return mName;
}



template <typename T>
QDebug &SyncParameter<T>::toDebug(QDebug &d) const
{
	const QString localValueString=valueToString(mLocalValue);
	const QString remoteValueString=valueToString(mRemoteValue);
	d.nospace() << "SyncParameter "<< mName <<" [ localValue=" << localValueString << " (" << mLocalTimestamp << "), remoteValue=" << remoteValueString << " (" << mRemoteTimestamp << "), needToSendAck=" << (needToSendAck()?"true":"false")  << ", needToSendDataAndReceiveAck=" << (needToSendDataAndReceiveAck()?"true":"false") <<"]";//, ctx(now=" << (mCTX.now()) << ", rtt=" << mCTX.roundTripTime() << ")]";
	return d.maybeSpace();
}


template <typename T>
void SyncParameter<T>::forceSync()
{
	qDebug()<<" --==## %% % FORCE SYNC";
	mNeedToSendAck=true;
	mNeedToSendDataAndReceiveAck=true;
}

template <typename T>
QDataStream &SyncParameter<T>::send(QDataStream &ds)
{
	//qDebug()<<"TX "<<valueToString(bestValue(false));
	ds << bestValue(false);
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
bool SyncParameter<T>::needToSendAck() const
{
	return mNeedToSendAck;
}

template <typename T>
bool SyncParameter<T>::needToSendDataAndReceiveAck() const
{
	return mNeedToSendDataAndReceiveAck;
}


template <typename T>
void SyncParameter<T>::ackSent()
{
	if(!mNeedToSendAck) {
		qWarning()<<"WARNING: sent ack when not expecting it";
	}
	mNeedToSendAck=false;
}


template <typename T>
void SyncParameter<T>::ack()
{
	if(!mNeedToSendDataAndReceiveAck) {
		qWarning()<<"WARNING: received ack when not expecting it";
	}
	mNeedToSendDataAndReceiveAck=false;
}

#endif // SYNCPARAMETER_HPP

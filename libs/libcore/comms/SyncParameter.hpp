#ifndef SYNCPARAMETER_HPP
#define SYNCPARAMETER_HPP

#include <QtGlobal>

class QDataStream;

class SyncContext
{
private:
	quint64 mNow;
	quint64 mRTT;

public:

	SyncContext(quint64 rtt, quint64 now=0);

	void updateNow(quint64 ts=0);

	quint64 roundTripTime() const;

	quint64 now() const;
};

class ISyncParameter
{
public:
	virtual QDataStream &operator<<(QDataStream &)=0;
	virtual QDataStream &operator>>(QDataStream &)=0;

};

template <typename T>
class SyncParameter: public ISyncParameter
{
private:

	T mLocalValue;
	T mRemoteValue;
	SyncContext &mCTX;
	quint64 mLocalTimestamp;
	quint64 mRemoteTimestamp;

public:

	SyncParameter(const T &val, SyncContext &ctx);
	void setLocalValue(const T &val);
	void setRemoteNoChange();
	void setRemoteValue(const T &val);
	T bestValue() const;
	bool isRemoteConfirmed() const;
	bool isLocalConfirmed() const;
	bool isInSync() const;
	T localValue();
	T remoteValue();
	quint64 localTimestamp();
	quint64 remoteTimestamp();
	QString toString() const;
	QDataStream &operator<<(QDataStream &ds) Q_DECL_OVERRIDE;
	QDataStream &operator>>(QDataStream &ds) Q_DECL_OVERRIDE;

};


#include <QDataStream>
#include <QString>



template <typename T>
SyncParameter<T>::SyncParameter(const T &val, SyncContext &ctx)
	: mLocalValue(val)
	, mRemoteValue(val)
	, mCTX(ctx)
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

template <typename T>
QString SyncParameter<T>::toString() const
{
	return QString("SyncParameter[ local=")+mLocalValue+"("+QString::number(mLocalTimestamp)+", sync="+(isLocalConfirmed()?"true":"false")+QString("), remote=")+mRemoteValue+"("+QString::number(mRemoteTimestamp)+", sync="+(isRemoteConfirmed()?"true":"false")+"), sync="+(isInSync()?"true":"false")+", ctx(now="+QString::number(mCTX.now())+", rtt="+QString::number(mCTX.roundTripTime())+")]";
}

template <typename T>
QDataStream &SyncParameter<T>::operator<<(QDataStream &ds)
{
	/*
	ds << AGENT_STATE;
	ds << sm.timestamp;
	ds << sm.mEmergencyShutdown;
	ds << sm.mMode;
	ds << sm.mTargetPosition;
	ds << sm.mTargetOrientation;
	ds << sm.mTargetPose;
	*/
	return ds;
}

template <typename T>
QDataStream &SyncParameter<T>::operator>>(QDataStream &ds)
{
	/*
	qint32 mt=INVALID;
	ds >> mt;
	sm.ok=(AGENT_STATE==mt);
	if(sm.ok)
	{
		ds >> sm.timestamp;
		ds >> sm.mEmergencyShutdown;
		ds >> sm.mMode;
		ds >> sm.mTargetPosition;
		ds >> sm.mTargetOrientation;
		ds >> sm.mTargetPose;
	}
	*/
	return ds;
}


#endif // SYNCPARAMETER_HPP

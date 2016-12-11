#ifndef SYNCCONTEXT_HPP
#define SYNCCONTEXT_HPP

#include "../libutil/utility/Standard.hpp"

#include <QVector>
#include <QBitArray>

class QDataStream;

class ISyncParameter;

template <typename T>
class SyncParameter;

class SyncContext
{
private:
	quint64 mNow;
	quint64 mRTT;
	QVector<ISyncParameter *> mParams;
	QBitArray mSyncBits;
	QBitArray mAckBits;
	quint16 mIdealSendingPayloadSize;
	quint16 mLastSendingPayloadSize;
	quint16 mLastReceivingPayloadSize;
	qint64 mAverageRoundTripTime;

public:

	SyncContext(quint64 rtt, quint64 now=0);

public:

	//Schedule a forced sync of ALL parmeters. Will incur bandwidth, please use with caution
	void forceSync(quint64 ts=0);
	void update(quint64 ts=0);
	quint64 roundTripTime() const;
	void setRoundTripTime(quint64 ms);
	quint64 now() const;

	template <typename T>
	SyncParameter<T> *registerParameter(const QString &name, const T &val);

	QString toString() const;

	QDebug &toDebug(QDebug &d) const;

	quint16 bytesSent() const;
	quint16 bytesSentIdeal() const;
	quint16 bytesRead() const;

	bool hasPendingSyncs() const;
	bool hasPendingAcks() const;

	QDataStream &send(QDataStream &ds);
	QDataStream &receive(QDataStream &ds);


};



QDebug &operator<<(QDebug &d, const SyncContext &sc);

inline QDataStream &operator<<(QDataStream &ds, SyncContext &sc)
{
	return sc.send(ds);
}

inline QDataStream &operator>>(QDataStream &ds, SyncContext &sc)
{
	return sc.receive(ds);
}


////////////////////////////////////////////////////////////////////////////////

template <typename T>
SyncParameter<T> * SyncContext::registerParameter(const QString &name, const T &val)
{
	OC_METHODGATE();
	SyncParameter<T> *param=new SyncParameter<T>(name, val, *this);
	mParams.append(param);
	//qDebug()<<"BEFORE REGISTERING PARAMETER: "<<mAckBits<<mSyncBits;
	mAckBits.resize(mAckBits.size()+1);
	mSyncBits.resize(mSyncBits.size()+1);
	mAckBits.setBit(mAckBits.size()-1,false);
	mSyncBits.setBit(mSyncBits.size()-1,false);
	//qDebug()<<"AFTER REGISTERING PARAMETER: "<<mAckBits<<mSyncBits;
	//qDebug()<<"LEFT "<<*this;
	return param;
}



#endif // SYNCCONTEXT_HPP

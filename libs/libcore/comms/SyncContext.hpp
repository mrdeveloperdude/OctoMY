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
	quint16 mPayloadSize;
	qint64 mAverageRoundTripTime;

public:

	SyncContext(quint64 rtt, quint64 now=0);

public:
	void update(quint64 ts=0);
	quint64 roundTripTime() const;
	void setRoundTripTime(quint64 ms);
	quint64 now() const;

	template <typename T>
	SyncParameter<T> *registerParameter(const T &val);

	QString toString() const;

	quint16 bytes() const;

	bool hasPendingSyncs() const;
	bool hasPendingAcks() const;

	QDataStream &operator<<(QDataStream &ds);
	QDataStream &operator>>(QDataStream &ds);

};



////////////////////////////////////////////////////////////////////////////////

template <typename T>
SyncParameter<T> * SyncContext::registerParameter(const T &val)
{
	OC_METHODGATE();
	SyncParameter<T> *param=new SyncParameter<T>(val, *this);
	mParams.append(param);
	mAckBits.resize(mAckBits.size()+1);
	mSyncBits.resize(mSyncBits.size()+1);
	mAckBits.setBit(mAckBits.size()-1,false);
	mSyncBits.setBit(mSyncBits.size()-1,true);
	return param;
}



#endif // SYNCCONTEXT_HPP

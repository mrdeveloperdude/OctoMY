#include "MockCommsCarrier.hpp"

#include "comms/PacketSendState.hpp"
#include "comms/address/CarrierAddressFactory.hpp"

#include <QHostAddress>

#include <QtGlobal>

MockCommsCarrier::MockCommsCarrier(QObject *parent)
	: CommsCarrier(parent)
	, mHasPendingData(false)
	, mWriteBatchSize(-1)
	, mIsStarted(false)
	, mStartFail(false)
	, mMinimalPacketInterval(100) // Shortest pause between sending opportunities is 100 ms (Throttling)
	, mMaximalPacketInterval(5000) // Longest pause between sending opportunities is 5000 ms (keepalive)
	, mOverrideStartStop(false)
{

}


MockCommsCarrier::~MockCommsCarrier()
{
	qDebug()<<"Mock-dtor";
}


////////////////////////// Mock interface


void MockCommsCarrier::mockWriteMock(QByteArray data, QSharedPointer<CarrierAddress> address, bool sendReadyReadSignal)
{
	QString key = address.isNull()?"":address->toString();
	mMockReadData.append(QPair<QString, QByteArray>(key, data));
	qDebug()<<"Mock-writing "<< data.size() << "bytes of data to be read from '" << key << "'";
	if(sendReadyReadSignal) {
		mHasPendingData=true;
		emit carrierReadyRead();
	}
}


QByteArray MockCommsCarrier::mockReadMock(QSharedPointer<CarrierAddress> address)
{
	const QString key = address.isNull()?"":address->toString();
	if(mMockWriteData.contains(key) && ! mMockWriteData[key].isEmpty()) {
		QByteArray out = mMockWriteData[key].takeFirst();
		qDebug()<<"Mock-reading "<< out.size() << "bytes of data to be written to '" << key << "'";
		return out;
	}
	return QByteArray();
}


void MockCommsCarrier::mockSetHasPendingData(bool hasPendingData)
{
	qDebug()<<"Mock-Setting HasPendingData to "<< hasPendingData;
	mHasPendingData=hasPendingData;
}


void MockCommsCarrier::mockSetWriteBatchSize(qint64 writeBatchSize)
{
	qDebug()<<"Mock-Setting WriteBatchSize to "<< writeBatchSize;
	mWriteBatchSize=writeBatchSize;
}


void MockCommsCarrier::mockSetStartFail(bool startFail)
{
	qDebug()<<"Mock-Setting StartFail to "<< startFail;
	mStartFail =	startFail;
}


void MockCommsCarrier::mockSetMinimalPacketInterval(quint64 size)
{
	qDebug()<<"Mock-Setting MinimalPacketInterval to "<< size;
	mMinimalPacketInterval = size;
}


void MockCommsCarrier::mockSetMaximalPacketIntervalImp(quint64 size)
{
	qDebug()<<"Mock-Setting MaximalPacketInterval to "<< size;
	mMaximalPacketInterval = size;
}


void MockCommsCarrier::mockSetAddress(QSharedPointer<CarrierAddress> addr)
{
	qDebug() << "Mock-Setting OurAddress to "<< (addr.isNull()?"NULL":addr->toString());
	mOurAddress = addr;
}


void MockCommsCarrier::mockSetOverrideSendingtimer(bool override)
{
	qDebug()<<"Mock-Setting override sending timer from "<<mOverrideStartStop<<" to "<<override;
	mOverrideStartStop = override;
}


void MockCommsCarrier::mockStartSendingTimer()
{
	qDebug()<<"Mock-StartSendingTimer";
	mSendingTimer.start();
}


void MockCommsCarrier::mockStopSendingTimer()
{
	qDebug()<<"Mock-StopSendingTimer";
	mSendingTimer.stop();
}


void MockCommsCarrier::mockTriggerReadyRead()
{
	qDebug()<<"Mock-Triggering ready read signal";
	emit carrierReadyRead();
}


void MockCommsCarrier::mockTriggerErrorSignal(QString error)
{
	qDebug()<<"Mock-Triggering error signal with message '"<<error<<"'";
	mLastError=error;
	emit carrierError(error);
}


void MockCommsCarrier::mockTriggerSendingOpportunity(quint64 now)
{
	qDebug().noquote().nospace()<<"Mock-Triggering sending opportunity signal with now='"<<now<<"'";
	emit carrierSendingOpportunity(now);
}


void MockCommsCarrier::mockTriggerConnectionStatusChanged(bool connected)
{
	qDebug()<<"Mock-Triggering connection status changed signal with new status='"<<connected<<"'";
	emit carrierConnectionStatusChanged(connected);
}


////////////////////////// CommsCarrier overrides to take countrol over sending opportunity timer


bool MockCommsCarrier::start(QSharedPointer<CarrierAddress> address)
{
	if(mOverrideStartStop) {
		return isConnectionMaintained();
	} else {
		CommsCarrier::setListenAddress(address);
		CommsCarrier::maintainConnection(true);
		return true;
	}
}


void MockCommsCarrier::stop()
{
	if(mOverrideStartStop) {
		return;
	} else {
		CommsCarrier::maintainConnection(false);
	}
}


//////////////////////////  CommsCarrier internal interface methods

void MockCommsCarrier::configureImp(){
	qDebug() << "configure() called";
}

bool MockCommsCarrier::activateImp(bool start)
{
	const bool oldIsStarted=mIsStarted;
	if(start) {
		if(!mStartFail) {
			mIsStarted=true;
		}
	} else {
		mIsStarted=false;
	}
	
	qDebug() << "activate() called with start= "<<start<<" for address " << (mOurAddress.isNull()?"":mOurAddress->toString()) << " with mock-fail=" << mStartFail << " and isStarted " << oldIsStarted << " --> " << mIsStarted;
	return mIsStarted;
}


void MockCommsCarrier::setAddressImp(QSharedPointer<CarrierAddress> address)
{
	qDebug() << "setAddress() called for address " << (address.isNull()?"":address->toString());
	mOurAddress = address;
}



bool MockCommsCarrier::isActiveImp() const
{
	return mIsStarted;
}


qint64 MockCommsCarrier::writeDataImp(const QByteArray &datagram, QSharedPointer<CarrierAddress> address)
{
	if(address.isNull()){
		qWarning() << "write() called. No address, wrote 0.";
		return 0;
	}
	const qint64 sz=datagram.size();
	const qint64 written=(mWriteBatchSize>=0)?qMin(mWriteBatchSize, sz):sz;
	if(written > 0) {
		auto l=datagram.left(static_cast<int>(written));
		QString key=address->toString();
		mMockWriteData[key].append(l);
	}
	qDebug() << "write() called. wrote " << written << " of " << sz << "bytes to address=" << (address.isNull()?"":address->toString()) << " (mock batch size=" << ( (mWriteBatchSize<0)?"no batch":QString::number(mWriteBatchSize)) << ")";
	return written;
}


qint64 MockCommsCarrier::readDataImp(char *data, qint64 maxlen, QHostAddress *host, quint16 *port)
{
	qDebug() << "read() called with maxlen="<<maxlen<<", data="<<(nullptr!=data)<<", host="<<(nullptr!=host)<<" and port="<<(nullptr!=port);
	if(mMockReadData.isEmpty()) {
		qDebug() << "read() returning 0 bytes as no data was available";
		return 0;
	}
	QPair<QString, QByteArray> pair = mMockReadData.takeFirst();
	const QString &key = pair.first;
	CarrierAddressFactory caf;
	auto na = caf.fromString(key);
	QByteArray all = pair.second;
	const qint64 sz = all.size();
	const quint64 bytesRead = static_cast<quint64>(qMin(maxlen, sz));
	QByteArray send = all.left(static_cast<int>(bytesRead));
	// Toss back the remaining data if any
	QByteArray keep = all.right(static_cast<qint64>(sz-bytesRead));
	if(keep.size()>0) {
		pair.second = keep;
		qDebug() << "PREPENDING REST";
		mMockReadData.prepend(pair);
	}
	
	
	//TODO: Fix this after CarrierAddress refactor
	/*
	// Send the requested number of bytes
	if(nullptr != all && bytesRead > 0) {
		memcpy(data, send.constData(), bytesRead);
	}
	// Send the host
	if(nullptr != host) {
		*host=QHostAddress(na->ip());
	}
	// Send the port
	if(nullptr != port) {
		*port=na->port();
	}*/
	qDebug() << "read() returning bytesRead="<<bytesRead<<", host="<<(nullptr!=host?host->toString():"nullptr")<<" and port="<<(nullptr!=port?QString::number(*port):"nullptr");
	return static_cast<qint64>(bytesRead);
}


bool MockCommsCarrier::hasPendingDataImp()
{
	return (!mMockReadData.isEmpty());
}


qint64 MockCommsCarrier::pendingDataSizeImp()
{
	if(mMockReadData.isEmpty()) {
		qDebug()<<"WARNING: Asking for pending size when no pending data";
		return 0;
	}
	const QPair<QString, QByteArray> &pair=mMockReadData.first();
	return pair.second.size();
}


QString MockCommsCarrier::errorStringImp()
{
	return mLastError;
}


QSharedPointer<CarrierAddress> MockCommsCarrier::addressImp()
{
	return mOurAddress;
}


quint64 MockCommsCarrier::minimalPacketIntervalImp()
{
	return mMinimalPacketInterval;
}


quint64	MockCommsCarrier::maximalPacketIntervalImp()
{
	return mMaximalPacketInterval;
}

#ifndef MOCKCOURIER_HPP
#define MOCKCOURIER_HPP

#include "comms/couriers/Courier.hpp"

#include <QObject>

class Comms;
class CommsTester;

class MockCourier:public Courier
{
	Q_OBJECT

public:
	CommsTester *mCt;
	quint16 mSoFar;

	const QByteArray mDatagram;
	const qint32 mMaxSends;
	const qint32 mMaxRecs;
	qint32 mSendCount;
	qint32 mRecCount;
	CourierMandate mMandate;

public:
	explicit MockCourier(QString name, QString dest, QByteArray datagram, QSharedPointer<Comms> comms, const qint32 mMaxSends=1, const qint32 mMaxRecs=1, CommsTester *parent = nullptr);
	virtual ~MockCourier() override;

public:
	//Let the CommChannel know what we want
	CourierMandate mandate() const override;

	//Override to act on sending opportunity.
	//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
	quint16 sendingOpportunity(QDataStream &ds)	override;

	//Override to act on data received
	//Return number of bytes actually read.
	quint16 dataReceived(QDataStream &ds, quint16 availableBytes) override;

	void writeSummary();
};


#endif
// MOCKCOURIER_HPP

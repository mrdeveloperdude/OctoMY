#ifndef COURIER
#define COURIER

#include "comms/CommsSession.hpp"
#include "CourierMandate.hpp"

#include <QObject>
#include <QDataStream>

class CommsChannel;

/////////////////////////////////////////////////////////////////////////////


class Courier : public QObject
{
	Q_OBJECT
private:
	//Destination
	QString mDestination;
	//Channel
	QSharedPointer<CommsChannel> mComms;
	//Identification
	QString mName;
	quint32 mID;
	quint32 mSerial;
	static quint32 mCt;
	//Last oportunity time
	quint64 mLastOpportunity;

public:
	static const quint32 FIRST_USER_ID;

public:
	explicit Courier(QString name, quint32 id, QSharedPointer<CommsChannel> comms, QObject *parent=nullptr);
	virtual	~Courier() override;

public:
	void setDestination(const QString );
	const QString  &destination() const;

public:
	QSharedPointer<CommsChannel> comms() const;
	void setForwardRescheduleSignal(QObject &ob, bool fwd);
	quint64 lastOpportunity() const;
	void  setLastOpportunity(quint64 now);
	quint32 id() const;
	QString name() const;
	quint32 serial() const;
	QString toString() const;

signals:
	void reschedule(quint64);

// Courier interface
public:

// Update courier state when channel has opportunity
	virtual void update(quint64 now);

// Let the CommChannel know what we want
	virtual CourierMandate mandate() const;

// Override to act on sending opportunity.
// Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
	virtual quint16 sendingOpportunity(QDataStream &ds);

// Override to act on data received
// Return number of bytes actually read.
	virtual quint16 dataReceived(QDataStream &ds, quint16 availableBytes);

};

#endif
// COURIER

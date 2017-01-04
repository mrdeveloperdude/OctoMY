#ifndef COURIER
#define COURIER

#include "comms/CommsSession.hpp"



#include <QObject>
#include <QDataStream>

class CommsChannel;



#include "CourierMandate.hpp"


/////////////////////////////////////////////////////////////////////////////



class Courier : public QObject
{
	Q_OBJECT
protected:
	//Destination
	CommsSignature mDestination;
	//Channel
	CommsChannel *mComms;
	//Identification
	QString mName;
	quint32 mID;
	//Last oportunity time
	quint64 mLastOpportunity;


public:
	static const quint32 FIRST_USER_ID;

public:
	explicit Courier(QString mName, quint32 mID, QObject *parent=0);


public:



	void setForwardRescheduleSignal(QObject &ob, bool fwd);

	void registered(CommsChannel &comms);
	void unRegistered(CommsChannel &comms);

	quint64 lastOpportunity() const;

	void  setLastOpportunity(quint64 now);

	quint32 id()const;
	QString name() const;

	void setDestination(const CommsSignature);
	const CommsSignature &destination() const;

	CommsChannel *comms() const;
	bool isRegistered() const;

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





#endif // COURIER

#ifndef COURIER
#define COURIER

#include "comms/Client.hpp"
#include "comms/CommsChannel.hpp"


#include <QObject>
#include <QDataStream>

class CommsChannel;

struct CourierMandate{

		CourierMandate(const quint64 payloadSize=0 , const quint8 priority=0 , const quint64 interval=100000 , const bool active=false);
		//Number of bytes of desired payload from 0 to 400
		quint64 payloadSize;
		//Priority of this courier from 0 to 255 where lower number means higher
		// priority and couriers with the sam priority are treated equal
		quint8 priority;
		//Desired interval of sendings in milliseconds
		quint64 interval;
		//Last oportunity time
		quint64 lastOportunity;
		//Are we currently active
		bool active;
};



/////////////////////////////////////////////////////////////////////////////



class Courier : public QObject{
		Q_OBJECT
	protected:
		//Destination
		ClientSignature destination;
		//Channel
		CommsChannel *c;

	public:
		explicit Courier(QObject *parent=0);


	public:
		void registered(CommsChannel &c);
		void unRegistered(CommsChannel &c);

		void setDestination(const ClientSignature);
		const ClientSignature &getDestination() const;

		//Let the CommChannel know what we want
		virtual CourierMandate mandate();

		//Override to act on sending opportunity.
		//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
		virtual quint64 sendingOpportunity(QDataStream &ds, quint32 availableBytes);

};

#endif // COURIER


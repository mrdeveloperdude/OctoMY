#ifndef COURIER
#define COURIER
#include "CommsChannel.hpp"

#include <QObject>
#include <QDataStream>

class CommsChannel;

struct CourierMandate{

		CourierMandate(quint64 payloadSize=0 , quint8 priority=0 , quint64 interval=100000 , bool active=false);
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

class Courier : public QObject{
		Q_OBJECT
	protected:
		CommsChannel *c;

	public:
		explicit Courier(QObject *parent=0);
	public:
		void registered(CommsChannel &c);
		void unRegistered(CommsChannel &c);

		//Let the CommChannel know what we want
		virtual CourierMandate mandate();

		//Override to acton sending opportunity.
		//Return true if you took advantage of the opportunity
		virtual quint64 sendingOportunity(QDataStream &ds, quint32 availableBytes);

};

#endif // COURIER


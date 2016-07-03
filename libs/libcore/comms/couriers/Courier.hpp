#ifndef COURIER
#define COURIER

#include "comms/Client.hpp"



#include <QObject>
#include <QDataStream>

class CommsChannel;



struct CourierMandate{
		//Defaults reflect the common need; payload size must be specified, priority is casual, interval is 1 Hz and active is presumed to be yes by default
		CourierMandate(const quint16 payloadSize , const quint8 priority=10, const quint64 interval=1000, const bool receiveActive=true, const bool sendActive=true);
		//Number of bytes of desired payload from 0 to 400
		quint16 payloadSize;
		//Priority of this courier from 0 to 255 where lower number means lower
		// priority and couriers with the sam priority are treated equal
		quint8 priority;
		//Desired interval of sendings in milliseconds
		quint64 interval;
		//Are we currently active for receiving data?
		bool receiveActive;
		//Are we currently active for sending data?
		//NOTE: When courier reports "sendActive" that means that it wants to send,
		//      and so even if it writes 0 bytes, there will be a section id present
		//      in packet reserved for it with the 0 bytes following it
		bool sendActive;
};



/////////////////////////////////////////////////////////////////////////////



class Courier : public QObject{
		Q_OBJECT
	protected:
		//Destination
		ClientSignature destination;
		//Channel
		CommsChannel *c;
		//Identification
		QString name;
		quint32 id;
		//Last oportunity time
		quint64 lastOpportunity;


	public:
		static const quint32 FIRST_USER_ID;

	public:
		explicit Courier(QString name, quint32 id, QObject *parent=0);


	public:

		inline quint64 getLastOpportunity(){
			return lastOpportunity;
		}

		inline void  setLastOpportunity(quint64 now){
			lastOpportunity=now;
		}

		void registered(CommsChannel &c);
		void unRegistered(CommsChannel &c);

		inline quint32 getID()const {return id;}
		inline QString getName()const {return name;}

		void setDestination(const ClientSignature);
		const ClientSignature &getDestination() const;

		//Let the CommChannel know what we want
		virtual CourierMandate mandate();

		//Override to act on sending opportunity.
		//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
		virtual quint16 sendingOpportunity(QDataStream &ds);

		//Override to act on data received
		//Return number of bytes actually read.
		virtual quint16 dataReceived(QDataStream &ds, quint16 availableBytes);

};

#endif // COURIER


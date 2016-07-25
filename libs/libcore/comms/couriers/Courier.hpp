#ifndef COURIER
#define COURIER

#include "comms/Client.hpp"



#include <QObject>
#include <QDataStream>

class CommsChannel;



#include "CourierMandate.hpp"


/////////////////////////////////////////////////////////////////////////////



class Courier : public QObject{
		Q_OBJECT
	protected:
		//Destination
		ClientSignature mDestination;
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

		inline quint64 lastOpportunity(){
			return mLastOpportunity;
		}

		inline void  setLastOpportunity(quint64 now){
			mLastOpportunity=now;
		}

		void registered(CommsChannel &comms);
		void unRegistered(CommsChannel &comms);

		inline quint32 id()const {return mID;}
		inline QString name()const {return mName;}

		void setDestination(const ClientSignature);
		const ClientSignature &destination() const;

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


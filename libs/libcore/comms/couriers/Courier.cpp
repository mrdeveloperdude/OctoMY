
#include "Courier.hpp"

#include "comms/CommsChannel.hpp"

CourierMandate::CourierMandate(
		const quint16 payloadSize
		, const quint8 priority
		, const quint64 interval
		, const bool receiveActive
		, const bool sendActive
		)
	:  payloadSize(payloadSize)
	, priority(priority)
	, interval(interval)
	, receiveActive(receiveActive)
	, sendActive(sendActive)
{

}




/////////////////////////////////////////////////////////////////////////////

const quint32 Courier::FIRST_USER_ID=1024;


Courier::Courier(QString name, quint32 id, QObject *parent)
	: QObject(parent)
	, name(name)
	, id(id)
	, lastOpportunity(0)
{

}

void Courier::setDestination(const ClientSignature sig){
	destination=sig;
}

const ClientSignature &Courier::getDestination() const{
	return destination;
}


//Let the CommChannel know what we want
CourierMandate Courier::mandate(){
	return CourierMandate(0);
}

//Override to act on sending opportunity.
//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
quint16 Courier::sendingOpportunity(QDataStream &ds){
	(void)ds;
	return 0;
}


//Override to act on data received
//Return number of bytes actually read.
quint16 Courier::dataReceived(QDataStream &ds, quint16 availableBytes){
	(void)ds;
	(void)availableBytes;
	return 0;
}

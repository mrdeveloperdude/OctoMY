
#include "Courier.hpp"

CourierMandate::CourierMandate(
		const quint64 payloadSize
		, const quint8 priority
		, const quint64 interval
		, const bool active
		)
	:  payloadSize(payloadSize)
	, priority(priority)
	, interval(interval)
	, lastOportunity(0)
	, active(active)
{

}




/////////////////////////////////////////////////////////////////////////////



Courier::Courier(QObject *parent)
	: QObject(parent)
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
	return CourierMandate();
}

//Override to act on sending opportunity.
//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
quint64 Courier::sendingOpportunity(QDataStream &ds, quint32 availableBytes){
	(void)ds;
	(void)availableBytes;
	return 0;
}



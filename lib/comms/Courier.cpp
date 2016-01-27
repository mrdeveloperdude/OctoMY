
#include "Courier.hpp"
#include "CommsChannel.hpp"

CourierMandate::CourierMandate(
		quint64 payloadSize
		, quint8 priority
		, quint64 interval
		, bool active
		):
	payloadSize(payloadSize)
  , priority(priority)
  , interval(interval)
  , lastOportunity(0)
  , active(active)
{

}



Courier::Courier(QObject *parent) :
	QObject(parent){

}

//Let the CommChannel know what we want
CourierMandate Courier::mandate(){
	return CourierMandate();
}

//Override to acton sending opportunity.
//Return true if you took advantage of the opportunity
quint64 Courier::sendingOportunity(QDataStream &ds, quint32 availableBytes){
	(void)ds;
	return 0;
}



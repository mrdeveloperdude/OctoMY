#include "DiscoveryCourier.hpp"

#include "comms/discovery/DiscoveryParticipant.hpp"
#include "comms/messages/MessageType.hpp"

DiscoveryCourier::DiscoveryCourier(QObject *parent)
	: Courier("Discovery", Courier::FIRST_USER_ID+2, parent)
{

}


void DiscoveryCourier::registerDiscoveryAuthentication(DiscoveryParticipant &part){

	factors[part.fullPublicAddress()]=&part;
	order.append(&part);
}



//Let the CommChannel know what we want
CourierMandate DiscoveryCourier::mandate(){
	return CourierMandate(100);
}

//Override to act on sending opportunity.
//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
quint16 DiscoveryCourier::sendingOpportunity(QDataStream &ds){
	if(!order.isEmpty()){
		DiscoveryParticipant *part=order[0];
		if(nullptr!=part){
			QString fadr=part->fullPublicAddress();
			ds << fadr;
		}
	}
	(void)ds;
	return 0;
}




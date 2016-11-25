
#include "Courier.hpp"


#include "comms/CommsChannel.hpp"





/////////////////////////////////////////////////////////////////////////////

const quint32 Courier::FIRST_USER_ID=1024;


Courier::Courier(QString name, quint32 id, QObject *parent)
	: QObject(parent)
	, mName(name)
	, mID(id)
	, mLastOpportunity(0)
{

}

void Courier::setDestination(const ClientSignature sig){
	mDestination=sig;
}

const ClientSignature &Courier::destination() const{
	return mDestination;
}


//Let the CommChannel know what we want
CourierMandate Courier::mandate(){
	return CourierMandate(0);
}

//Override to act on sending opportunity.
//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
quint16 Courier::sendingOpportunity(QDataStream &ds){
	(void)ds;
	qWarning()<<"WARNING: Unimplemented sendingOpportunity() in "<<mName<<"("<<mID<<")";
	return 0;
}


//Override to act on data received
//Return number of bytes actually read.
quint16 Courier::dataReceived(QDataStream &ds, quint16 availableBytes){
	(void)ds;
	(void)availableBytes;
	qWarning()<<"WARNING: Unimplemented dataReceived() in "<<mName<<"("<<mID<<")";
	return 0;
}


#include "Courier.hpp"


#include "comms/CommsChannel.hpp"


#include "../libutil/utility/Standard.hpp"

#include <QString>



/////////////////////////////////////////////////////////////////////////////

const quint32 Courier::FIRST_USER_ID=1024;


Courier::Courier(QString name, quint32 id, QObject *parent)
	: QObject(parent)
	, mName(name)
	, mID(id)
	, mLastOpportunity(0)
{


}

void Courier::setDestination(const ClientSignature sig)
{
	mDestination=sig;
}

const ClientSignature &Courier::destination() const
{
	return mDestination;
}


void Courier::setForwardRescheduleSignal(QObject &ob, bool fwd)
{
	if(fwd) {
		if(!connect(&ob,SIGNAL(reschedule(quint64)),this,SIGNAL(reschedule(quint64)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
	} else {
		if(!disconnect(&ob,SIGNAL(reschedule(quint64)),this,SIGNAL(reschedule(quint64)))) {
			qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
		}
	}
}


quint64 Courier::lastOpportunity() const
{
	return mLastOpportunity;
}

void  Courier::setLastOpportunity(quint64 now)
{
	mLastOpportunity=now;
}



quint32 Courier::id()const
{
	return mID;
}
QString Courier::name() const
{
	return mName;
}


// Update courier state when channel has opportunity
void Courier::update(quint64 now)
{
	//It is perfectly fine to skip on this
	//qWarning()<<"WARNING: Unimplemented update() in "<<mName<<"("<<mID<<")";
}


// Let the CommChannel know what we want
CourierMandate Courier::mandate() const
{
	return CourierMandate(0);
}

// Override to act on sending opportunity.
// Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
quint16 Courier::sendingOpportunity(QDataStream &ds)
{
	(void)ds;
	qWarning()<<"WARNING: Unimplemented sendingOpportunity() in "<<mName<<"("<<mID<<")";
	return 0;
}


// Override to act on data received
// Return number of bytes actually read.
quint16 Courier::dataReceived(QDataStream &ds, quint16 availableBytes)
{
	(void)ds;
	(void)availableBytes;
	qWarning()<<"WARNING: Unimplemented dataReceived() in "<<mName<<"("<<mID<<")";
	return 0;
}

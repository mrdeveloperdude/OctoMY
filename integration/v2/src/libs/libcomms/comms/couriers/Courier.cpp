#include "Courier.hpp"

#include "comms/CommsChannel.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"


#include <QString>



/////////////////////////////////////////////////////////////////////////////

const quint32 Courier::FIRST_USER_ID=1024;

quint32 Courier::mCt=0;


Courier::Courier(QString name, quint32 id, QSharedPointer<CommsChannel> cc, QObject *parent)
	: QObject(parent)
	, mComms(cc)
	, mName(name)
	, mID(id)
	, mSerial(mCt++)
	, mLastOpportunity(0)
{
	OC_METHODGATE();
}

Courier::~Courier()
{
	OC_METHODGATE();
}

void Courier::setDestination(const QString sig)
{
	OC_METHODGATE();
	mDestination=sig;
}

const QString  &Courier::destination() const
{
	OC_METHODGATE();
	return mDestination;
}


QSharedPointer<CommsChannel> Courier::comms() const
{
	OC_METHODGATE();
	return mComms;
}

void Courier::setForwardRescheduleSignal(QObject &ob, bool fwd)
{
	OC_METHODGATE();
	if(fwd) {
		if(!connect(&ob, SIGNAL(reschedule(quint64)), this,SIGNAL(reschedule(quint64)), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
	} else {
		if(!disconnect(&ob, SIGNAL(reschedule(quint64)), this, SIGNAL(reschedule(quint64)))) {
			qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
		}
	}
}

quint64 Courier::lastOpportunity() const
{
	OC_METHODGATE();
	return mLastOpportunity;
}

void  Courier::setLastOpportunity(quint64 now)
{
	OC_METHODGATE();
	mLastOpportunity=now;
}

quint32 Courier::id()const
{
	OC_METHODGATE();
	return mID;
}

QString Courier::name() const
{
	OC_METHODGATE();
	return mName;
}

quint32 Courier::serial()const
{
	OC_METHODGATE();
	return mSerial;
}

QString Courier::toString() const
{
	OC_METHODGATE();
	QString out="Courier{name="+mName+", id="+QString::number(mID)+", dest= "+mDestination+", serial="+QString::number(mSerial)+", RX="+(mandate().receiveActive?"ON":"OFF")+", TX="+(mandate().sendActive?"ON":"OFF")+"}";
	return out;
}

// Update courier state when channel has opportunity
void Courier::update(quint64 now)
{
	OC_METHODGATE();
	Q_UNUSED(now);
	// NOTE: It is perfectly fine to skip on this
	//qWarning()<<"WARNING: Unimplemented update() in "<<mName<<"("<<mID<<")";
}


// Let the CommChannel know what we want
CourierMandate Courier::mandate() const
{
	OC_METHODGATE();
	return CourierMandate(0);
}

// Override to act on sending opportunity.
// Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
quint16 Courier::sendingOpportunity(QDataStream &ds)
{
	OC_METHODGATE();
	(void)ds;
	qWarning()<<"WARNING: Unimplemented sendingOpportunity() in "<<mName<<"("<<mID<<")";
	return 0;
}


// Override to act on data received
// Return number of bytes actually read.
quint16 Courier::dataReceived(QDataStream &ds, quint16 availableBytes)
{
	OC_METHODGATE();
	(void)ds;
	(void)availableBytes;
	qWarning()<<"WARNING: Unimplemented dataReceived() in "<<mName<<"("<<mID<<")";
	return 0;
}

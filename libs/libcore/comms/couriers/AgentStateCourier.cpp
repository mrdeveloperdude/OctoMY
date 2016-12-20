/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "AgentStateCourier.hpp"

#include "comms/SyncParameter.hpp"
#include "comms/couriers/CourierMandate.hpp"

#include "../libutil/utility/Standard.hpp"

#include <QDebug>
#include <QDataStream>
#include <QDateTime>



const quint32 AgentStateCourier::AGENT_STATE_COURIER_ID=(Courier::FIRST_USER_ID + 5);

// Agent side constructor
AgentStateCourier::AgentStateCourier(QDataStream *initialization, QObject *parent)
	: Courier("AgentState", AGENT_STATE_COURIER_ID, parent)
	, mAgentSide(nullptr!=initialization)
	, mParams(100)
	, mLastSendTime(0)
	, mLastChangeTime(0)
	, mFlags(nullptr)
	, mMode(nullptr)
	, mTargetPosition(nullptr)
	, mTargetOrientation(nullptr)
	, mTargetPose(nullptr)
{
	OC_METHODGATE();
	initParams(initialization);
	//mParams.forceSync(QDateTime::currentMSecsSinceEpoch());
	mParams.update(QDateTime::currentMSecsSinceEpoch());

	//setForwardRescheduleSignal(mParams, true);

	if(!connect(&mParams,SIGNAL(reschedule(quint64)),this,SIGNAL(reschedule(quint64)),OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect "<<mParams;
	}


}



void AgentStateCourier::setPanic(bool panic)
{
	OC_METHODGATE();
	quint8 flags=mFlags->localValue();
	if(panic) {
		flags=flags | 1;
	} else {
		flags=flags & (~1);
	}
	mFlags->setLocalValue(flags);

}

void AgentStateCourier::setFlags(quint8 flags)
{
	OC_METHODGATE();
	mFlags->setLocalValue(flags);
}


void AgentStateCourier::setMode(AgentMode mode)
{
	OC_METHODGATE();
	mMode->setLocalValue(mode);
}

void AgentStateCourier::setPoseValue(quint32 index, qreal value)
{
	OC_METHODGATE();
	if(nullptr!=mTargetPose) {
		Pose &pose=mTargetPose->localValueRef();
		pose.setValue(index,value);
		qDebug()<<"SETTING NEW POSE";
		mTargetPose->markLocalValueAsUpdated();
	}
}

bool AgentStateCourier::panic()
{
	OC_METHODGATE();
	if(nullptr!=mFlags) {
		return (mFlags->bestValue(true) & 0x1)>0;
	}
	//Missing the panic parmater is cause for panic, don't you think?
	return true;
}

QString AgentStateCourier::toString() const
{
	OC_METHODGATE();
	return "AgentStateCourier{ mandate="+mandate().toString()+", params="+mParams.toString()+" }";
}

QDebug &AgentStateCourier::toDebug(QDebug &d) const
{
	OC_METHODGATE();
	d << "AgentStateCourier{ mandate="+mandate().toString()+", params=";
	d << mParams;
	d << "}";
	return d;
}

void AgentStateCourier::initParams(QDataStream *initialization)
{
	OC_METHODGATE();
	//qDebug()<<"REGISTERING PARAMETERS with "<<(nullptr!=initialization?"DATA":"NULL");
	quint8 flags=0;
	mFlags=mParams.registerParameter("Flags", flags);
	if(nullptr==mFlags) {
		qWarning()<<"ERROR: could nota allocate flags parameter";
	} else {
		//qDebug()<<"REGISTERED PARAMETER #1: "<<mParams;
	}

	AgentMode mode=OFFLINE;
	mMode=mParams.registerParameter("Mode", mode);
	if(nullptr==mMode) {
		qWarning()<<"ERROR: could nota allocate agent mode parameter";
	} else {
		//qDebug()<<"REGISTERED PARAMETER #2: "<<mParams;
	}

	QGeoCoordinate targetPosition;
	mTargetPosition=mParams.registerParameter("TargetPosition", targetPosition);
	if(nullptr==mTargetPosition) {
		qWarning()<<"ERROR: could nota allocate target position parameter";
	} else {
		//qDebug()<<"REGISTERED PARAMETER #3: "<<mParams;
	}

	qreal targetOrientation=0;//Asume North at startup
	mTargetOrientation=mParams.registerParameter("TargetOrientation", targetOrientation);
	if(nullptr==mTargetOrientation) {
		qWarning()<<"ERROR: could nota allocate target orientation parameter";
	} else {
		//qDebug()<<"REGISTERED PARAMETER #4: "<<mParams;
	}
	Pose targetPose(32);
	mTargetPose=mParams.registerParameter("TargetPose", targetPose);
	if(nullptr==mTargetPose) {
		qWarning()<<"ERROR: could nota allocate target pose parameter";
	} else {
		//qDebug()<<"REGISTERED PARAMETER #5: "<<mParams;
	}

//Read initial data into parameters

//TODO: Fix and re-enable this. TIP: logging indicates the problem, ack and sync bit arrays are destroyed after load
	if(nullptr!=initialization) {
		/*
		qDebug()<<"BEFORE Reading in initial values for agent side parameters:"<<mParams;
		QDataStream &ds=*initialization;
		ds >> mParams;
		qDebug()<<"AFTER Reading in initial values for agent side parameters:"<<mParams;
		*/
	}

}


void AgentStateCourier::setHookSignals(QObject &ob, bool hook)
{
	OC_METHODGATE();
	if(nullptr!=mFlags) {
		mFlags->setHookSignals(ob, hook);
	} else {
		qWarning()<<"ERROR: No flags while hooking signals "<<hook;
	}
	if(nullptr!=mMode) {
		mMode->setHookSignals(ob, hook);
	} else {
		qWarning()<<"ERROR: No mode while hooking signals "<<hook;
	}
	if(nullptr!=mTargetPosition) {
		mTargetPosition->setHookSignals(ob, hook);
	} else {
		qWarning()<<"ERROR: No target pos while hooking signals "<<hook;
	}
	if(nullptr!=mTargetOrientation) {
		mTargetOrientation->setHookSignals(ob, hook);
	} else {
		qWarning()<<"ERROR: No target orientation while hooking signals "<<hook;
	}
	if(nullptr!=mTargetPose) {
		mTargetPose->setHookSignals(ob, hook);
	} else {
		qWarning()<<"ERROR: No target pose while hooking signals "<<hook;
	}
}



const QDebug &operator<<(QDebug &d, const AgentStateCourier &a)
{
	OC_FUNCTIONGATE();
	return a.toDebug(d);
}

////////////////////////////////////////////////////////////////////////////////
// Courier Interface ///////////////////////////////////////////////////////////

// Update courier state when channel has opportunity
void AgentStateCourier::update(quint64 now)
{
	OC_METHODGATE();
	mParams.update(now);
}


// Let the CommChannel know what we want
CourierMandate AgentStateCourier::mandate() const
{
	OC_METHODGATE();
	CourierMandate mandate(mParams.bytesSentIdeal() , 10, 50, true, mParams.hasPendingSyncs() || mParams.hasPendingAcks() );
	return mandate;
}

// Act on sending opportunity.
// Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
quint16 AgentStateCourier::sendingOpportunity(QDataStream &ds)
{
	OC_METHODGATE();
	if(mandate().sendActive) {
		ds << mParams;
		quint16 bytes=mParams.bytesSent();
		//qDebug()<<"Spent sending opportunity for agent status data with "<<bytes<<" bytes";
		return  bytes;
	} else {
		qWarning()<<"ERROR: sendingOpportunity while sendActive=false";
	}
	return 0;
}


quint16 AgentStateCourier::dataReceived(QDataStream &ds, quint16 availableBytes)
{
	OC_METHODGATE();
	if(mandate().receiveActive) {
		ds >> mParams;
		quint16 bytes=mParams.bytesRead();
		//qDebug()<<"Receiving "<<bytes<<" of "<<availableBytes<<" bytes "<<(mAgentSide?"AGENT SIDE":"CONTROL SIDE");
		return  bytes;
	} else {
		qWarning()<<"ERROR: dataReceived while receiveActive=false";
	}
	return 0;
}

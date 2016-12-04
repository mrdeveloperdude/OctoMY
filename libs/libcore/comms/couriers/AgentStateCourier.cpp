#include "AgentStateCourier.hpp"

#include "comms/SyncParameter.hpp"

#include <QDebug>

#include <QDataStream>

const quint32 AgentStateCourier::AGENT_STATE_COURIER_ID=(Courier::FIRST_USER_ID + 5);

// Agent side constructor
AgentStateCourier::AgentStateCourier(QDataStream *initialization, QObject *parent)
	: Courier("AgentState", AGENT_STATE_COURIER_ID, parent)
	, mAgentSide(nullptr!=initialization)
	, mSync(100)
	, mLastSendTime(0)
	, mLastChangeTime(0)
	, mFlags(nullptr)
	, mMode(nullptr)
	, mTargetPosition(nullptr)
	, mTargetOrientation(nullptr)
	, mTargetPose(nullptr)
{
	initParams(initialization);
}


void AgentStateCourier::initParams(QDataStream *initialization)
{
	quint8 flags=0;
	mFlags=mSync.registerParameter(flags);
	if(nullptr==mFlags) {
		qWarning()<<"ERROR: could nota allocate flags parameter";
	}
	AgentMode mode=OFFLINE;
	mMode=mSync.registerParameter(mode);
	if(nullptr==mMode) {
		qWarning()<<"ERROR: could nota allocate agent mode parameter";
	}
	QGeoCoordinate targetPosition;
	mTargetPosition=mSync.registerParameter(targetPosition);
	if(nullptr==mTargetPosition) {
		qWarning()<<"ERROR: could nota allocate target position parameter";
	}
	qreal targetOrientation=0;//Asume North at startup
	mTargetOrientation=mSync.registerParameter(targetOrientation);
	if(nullptr==mTargetOrientation) {
		qWarning()<<"ERROR: could nota allocate target orientation parameter";
	}
	Pose targetPose;
	mTargetPose=mSync.registerParameter(targetPose);
	if(nullptr==mTargetPose) {
		qWarning()<<"ERROR: could nota allocate target pose parameter";
	}

	//Read initial data into parameters
	if(nullptr!=initialization) {
		QDataStream &ds=*initialization;
		mSync.operator >>(ds);
	}

}

////////////////////////////////////////////////////////////////////////////////
// Courier Interface ///////////////////////////////////////////////////////////

//Let the CommChannel know what we want
CourierMandate AgentStateCourier::mandate()
{
	CourierMandate mandate(mSync.bytes() , 10, 100, true, mSync.hasPendingSyncs());
	return mandate;
}

//Act on sending opportunity.
//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
quint16 AgentStateCourier::sendingOpportunity(QDataStream &ds)
{
	if(mandate().sendActive) {
		qDebug()<<"Spending sending opportunity for agent status data";
		mSync.operator <<(ds);
		const quint16 bytes=mSync.bytes();
		return bytes;
	}
	qWarning()<<" + return 0 (default)";
	return 0;
}


quint16 AgentStateCourier::dataReceived(QDataStream &ds, quint16 availableBytes)
{
	if(mandate().receiveActive) {
		qDebug()<<"Receiving agent status data";
		const quint16 bytes=mandate().payloadSize;
		if(availableBytes>bytes) {
			qWarning()<<" + return 0 (message bigger than payload mandate size)";
			return 0;
		}
		mSync.operator >>(ds);
		return bytes;
	}
	qWarning()<<" + return 0 (default)";
	return 0;
}

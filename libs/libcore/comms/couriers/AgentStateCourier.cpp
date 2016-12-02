#include "AgentStateCourier.hpp"

#include <QDebug>


const quint32 AgentStateCourier::AGENT_STATE_COURIER_ID=(Courier::FIRST_USER_ID + 5);

// Agent side constructor
AgentStateCourier::AgentStateCourier(QDataStream &initialization, QObject *parent)
	: Courier("AgentState", AGENT_STATE_COURIER_ID, parent)
	, mAgentSide(true)
	, mMandate(400)
	, mMessage(initialization)
	, mLastSendTime(0)
{

}

// Controller side constructor
AgentStateCourier::AgentStateCourier(QObject *parent)
	: Courier("AgentState", AGENT_STATE_COURIER_ID, parent)
	, mAgentSide(false)
	, mMandate(400)
	, mLastSendTime(0)
{

}

//Let the CommChannel know what we want
CourierMandate AgentStateCourier::mandate()
{
	return mMandate;
}

//Act on sending opportunity.
//Return nubmer of bytes sent ( >0 ) if you took advantage of the opportunity
quint16 AgentStateCourier::sendingOpportunity(QDataStream &ds)
{
	if(mMandate.sendActive) {
		qDebug()<<"Spending sending opportunity for agent status data";
		const quint32 bytes=mMessage.bytes();
		if(bytes>mMandate.payloadSize) {
			qWarning()<<" + return 0 (message bigger than payload mandate size)";
			return 0;
		}
		ds<<mMessage;
		mMandate.sendActive=false;
		return bytes;
	}
	qWarning()<<" + return 0 (default)";
	return 0;
}


quint16 AgentStateCourier::dataReceived(QDataStream &ds, quint16 availableBytes)
{
	if(mMandate.receiveActive) {
		qDebug()<<"Receiving agent status data";
		if(availableBytes>mMandate.payloadSize) {
			qWarning()<<" + return 0 (message bigger than payload mandate size)";
			return 0;
		}
		ds>>mMessage;
		return mMessage.bytes();
	}
	qWarning()<<" + return 0 (default)";
	return 0;
}

void AgentStateCourier::onStatusChanged()
{
	//mMessage;
	mMandate.sendActive=true;
}

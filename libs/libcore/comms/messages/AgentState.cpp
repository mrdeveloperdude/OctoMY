#include "AgentState.hpp"

#include "MessageType.hpp"

#include <QDebug>


AgentState::AgentState()
{

}

AgentState::AgentState(QDataStream &ds)
{
	ds >> *this;
}


qint64 AgentState::bytes()
{
	if(sz<0) {
		QByteArray ba;
		{
			QDataStream ds(&ba,QIODevice::WriteOnly);
			ds<<this;
		}
		sz=ba.size();
		//qDebug()<<"Counted bytesize of StatusMessage to be "<<sz;
	}
	return sz;

}


QDataStream &operator>>(QDataStream &ds, AgentState &sm)
{
	qint32 mt=INVALID;
	ds >> mt;
	sm.ok=(AGENT_STATE==mt);
	if(sm.ok) {
		ds >> sm.timestamp;


		ds >> sm.mEmergencyShutdown;
		ds >> sm.mMode;
		ds >> sm.mTargetPosition;
		ds >> sm.mTargetOrientation;
		ds >> sm.mTargetPose;




	}
	return ds;
}

QDataStream &operator<<(QDataStream &ds, const AgentState &sm)
{
	ds << AGENT_STATE;
	ds << sm.timestamp;

	ds << sm.mEmergencyShutdown;
	ds << sm.mMode;
	ds << sm.mTargetPosition;
	ds << sm.mTargetOrientation;
	ds << sm.mTargetPose;

	return ds;
}



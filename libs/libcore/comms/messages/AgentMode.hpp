#ifndef AGENTMODE_HPP
#define AGENTMODE_HPP

#include <QDebug>


enum AgentMode {
	OFFLINE=0 // Robot is shut down
	, COMPLETE_AUTONOMY // All decisions are made by robot on it's own
	, AUTO_PILOT // Instructions are given as high-level objectives that the robot tries to follow on it's own
	, MANUAL_PILOT // Instructions are given to drive the robot directly.
};


QDebug &operator<<(QDebug &d, const AgentMode &);



QString AgentModeToString(AgentMode s);


AgentMode AgentModeFromString(QString s);




QDataStream &operator<<(QDataStream &, const AgentMode &);
QDataStream &operator>>(QDataStream &, AgentMode &);

int bytes(const AgentMode &);

#endif // AGENTMODE_HPP

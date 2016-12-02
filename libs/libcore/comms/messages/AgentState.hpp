#ifndef AGENTSTATE_HPP
#define AGENTSTATE_HPP

#include "puppet/Pose.hpp"

#include "AgentMode.hpp"

#include <QGeoCoordinate>

#include <QDataStream>



class AgentState
{
private:
	qint64 sz=-1;
	bool ok;
	qint64 timestamp;
	bool mEmergencyShutdown; // When true this robot is completely shut down. It can be triggered automatically when faults are found (to uphold Asimov's first law) or manually either by remote or on site.
	AgentMode mMode; // The desired mode of operation for this robot
	QGeoCoordinate mTargetPosition; // The desired target position for the robot in lat,long,alt. The robot may or may not actively pursue this location depending on the mode it is in.
	qreal mTargetOrientation; // The desired target orientation for the robot in radians starting from North. The robot may or may not actively pursue this orientation depending on the mode it is in.
	Pose mTargetPose; // The desired target pose for the robot in actuator positions. The robot may or may not actively pursue this pose depending on the mode it is in.
	//TODO: Somehow encode desired speed, direction and more

public:
	AgentState();
	AgentState(QDataStream &);

	qint64 bytes();

private:

	friend 	QDataStream &operator<<(QDataStream &, const AgentState &);
	friend QDataStream &operator>>(QDataStream &, AgentState &);
};




QDataStream &operator<<(QDataStream &, const AgentState &);
QDataStream &operator>>(QDataStream &, AgentState &);





#endif // AGENTSTATE_HPP

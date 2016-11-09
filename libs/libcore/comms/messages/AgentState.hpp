#ifndef AGENTSTATE_HPP
#define AGENTSTATE_HPP

#include "puppet/Pose.hpp"

#include <QGeoPositionInfo>

enum AgentMode {
	OFFLINE // Robot is shut down
	, COMPLETE_AUTONOMY // All decisions are made by robot on it's own
	, AUTO_PILOT // Instructions are given as high-level objectives that the robot tries to follow on it's own
	, MANUAL_PILOT // Instructions are given to drive the robot directly.
};

class AgentState
{
private:

	bool mEmergencyShutdown; // When true this robot is completely shut down. It can be triggered automatically when faults are found (to uphold Asimov's first law) or manually either by remote or on site.

	AgentMode mMode; // The desired mode of operation for this robot

	QGeoPositionInfo mTargetPosition; // The desired target position for the robot. The robot may or may not actively pursue this location depending on the mode it is in.

	qreal mTargetOrientation; // The desired target orientation for the robot. The robot may or may not actively pursue this orientation depending on the mode it is in.

	Pose mTargetPose; // The desired target pose for the robot. The robot may or may not actively pursue this pose depending on the mode it is in.

	//TODO: Somehow encode desired speed, direction,

public:
	AgentState();
};

#endif // AGENTSTATE_HPP

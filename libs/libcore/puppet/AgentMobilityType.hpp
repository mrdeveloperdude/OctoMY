#ifndef AGENTMOBILITYTYPE_HPP
#define AGENTMOBILITYTYPE_HPP

#include <QDebug>

/*!
 * \brief The AgentMobilityType enum is described here http://www.octomy.org/documentation/supported-hardware/robot-classes
 */

enum AgentMobilityType {

	MOBILITY_STATIONARY		// This robot is not mobile
	, MOBILITY_SUBMARINE	// This robot travels under water
	, MOBILITY_MARINE		// This robot travels on the surface of water
	, MOBILITY_THREADS		// This robot travels on land using threads
	, MOBILITY_WHEELS		// This robot travels on land using wheels
	, MOBILITY_HEXAPOD		// This robot travels on land using 6 legs
	, MOBILITY_QUADRUPED	// This robot travels on land using 4 legs
	, MOBILITY_BIPED		// This robot travels on land using 2 legs
	, MOBILITY_QUADCOPTER	// This robot travels in the air using 4 vertical propellers
	, MOBILITY_HELICOPTER	// This robot travels in the air using one horizontal and one vertical propellers
	, MOBILITY_WINGED		// This robot travels in the air using wings
	, MOBILITY_ROCKET		// This robot travels in the air using rocket propulsion

};




const QDebug &operator<<(QDebug &d, const AgentMobilityType &type);



QString AgentMobilityTypeToString(AgentMobilityType s);
AgentMobilityType AgentMobilityTypeFromString(QString s);



#endif // AGENTMOBILITYTYPE_HPP

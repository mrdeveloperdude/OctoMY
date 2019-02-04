#include "AgentMobilityType.hpp"


const QDebug &operator<<(QDebug &d, const AgentMobilityType &type)
{

#define  AgentMobilityTypeStreamOutCASE(A) case(A): { d.nospace() << #A; }break;
	switch(type) {
		AgentMobilityTypeStreamOutCASE(MOBILITY_STATIONARY);
		AgentMobilityTypeStreamOutCASE(MOBILITY_SUBMARINE);
		AgentMobilityTypeStreamOutCASE(MOBILITY_MARINE);
		AgentMobilityTypeStreamOutCASE(MOBILITY_THREADS);
		AgentMobilityTypeStreamOutCASE(MOBILITY_WHEELS);
		AgentMobilityTypeStreamOutCASE(MOBILITY_HEXAPOD);
		AgentMobilityTypeStreamOutCASE(MOBILITY_QUADRUPED);
		AgentMobilityTypeStreamOutCASE(MOBILITY_BIPED);
		AgentMobilityTypeStreamOutCASE(MOBILITY_QUADCOPTER);
		AgentMobilityTypeStreamOutCASE(MOBILITY_HELICOPTER);
		AgentMobilityTypeStreamOutCASE(MOBILITY_WINGED);
		AgentMobilityTypeStreamOutCASE(MOBILITY_ROCKET);

	default:
		d.nospace() << "MOBILITY_UNKNOWN";
	}
	return d.maybeSpace();
#undef AgentMobilityTypeStreamOutCASE
}


QString AgentMobilityTypeToString(AgentMobilityType s)
{
#define  AgentMobilityTypeToStringCASE(A) case (A):return #A
	switch(s) {
	default:
		return "UNKNOWN";
		AgentMobilityTypeToStringCASE(MOBILITY_STATIONARY);
		AgentMobilityTypeToStringCASE(MOBILITY_SUBMARINE);
		AgentMobilityTypeToStringCASE(MOBILITY_MARINE);
		AgentMobilityTypeToStringCASE(MOBILITY_THREADS);
		AgentMobilityTypeToStringCASE(MOBILITY_WHEELS);
		AgentMobilityTypeToStringCASE(MOBILITY_HEXAPOD);
		AgentMobilityTypeToStringCASE(MOBILITY_QUADRUPED);
		AgentMobilityTypeToStringCASE(MOBILITY_BIPED);
		AgentMobilityTypeToStringCASE(MOBILITY_QUADCOPTER);
		AgentMobilityTypeToStringCASE(MOBILITY_HELICOPTER);
		AgentMobilityTypeToStringCASE(MOBILITY_WINGED);
		AgentMobilityTypeToStringCASE(MOBILITY_ROCKET);
	}
#undef AgentMobilityTypeToStringCASE
}



AgentMobilityType AgentMobilityTypeFromString(QString s)
{
#define  AgentMobilityTypeFromStringCASE(A) if(#A==s) { return A; }

	AgentMobilityTypeFromStringCASE(MOBILITY_STATIONARY);
	AgentMobilityTypeFromStringCASE(MOBILITY_SUBMARINE);
	AgentMobilityTypeFromStringCASE(MOBILITY_MARINE);
	AgentMobilityTypeFromStringCASE(MOBILITY_THREADS);
	AgentMobilityTypeFromStringCASE(MOBILITY_WHEELS);
	AgentMobilityTypeFromStringCASE(MOBILITY_HEXAPOD);
	AgentMobilityTypeFromStringCASE(MOBILITY_QUADRUPED);
	AgentMobilityTypeFromStringCASE(MOBILITY_BIPED);
	AgentMobilityTypeFromStringCASE(MOBILITY_QUADCOPTER);
	AgentMobilityTypeFromStringCASE(MOBILITY_HELICOPTER);
	AgentMobilityTypeFromStringCASE(MOBILITY_WINGED);
	AgentMobilityTypeFromStringCASE(MOBILITY_ROCKET);

#undef AgentMobilityTypeFromStringCASE


	return MOBILITY_STATIONARY;
}


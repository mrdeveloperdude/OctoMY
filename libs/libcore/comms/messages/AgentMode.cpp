#include "AgentMode.hpp"

#include <QDataStream>


QDebug &operator<<(QDebug &d, const AgentMode &mode)
{
#define  AgentModeOperatorStreamLeftCASE(A) case (A):{d.nospace() << #A;}break
	switch(mode) {
		AgentModeOperatorStreamLeftCASE(OFFLINE);
		AgentModeOperatorStreamLeftCASE(COMPLETE_AUTONOMY);
		AgentModeOperatorStreamLeftCASE(AUTO_PILOT);
		AgentModeOperatorStreamLeftCASE(MANUAL_PILOT);
	default:
		d.nospace() << "UNKNOWN";
	}
	return d.maybeSpace();
#undef AgentModeOperatorStreamLeftCASE
}


QString AgentModeToString(AgentMode s)
{
#define  AgentModeToStringCASE(A) case (A):return #A
	switch(s) {
	default:
		return "UNKNOWN";
		AgentModeToStringCASE(OFFLINE);
		AgentModeToStringCASE(COMPLETE_AUTONOMY);
		AgentModeToStringCASE(AUTO_PILOT);
		AgentModeToStringCASE(MANUAL_PILOT);

	}
#undef AgentModeToStringCASE
}



AgentMode AgentModeFromString(QString s)
{
#define  AgentModeFromStringCASE(A) if(#A==s)return A
	AgentModeFromStringCASE(OFFLINE);
	AgentModeFromStringCASE(COMPLETE_AUTONOMY);
	AgentModeFromStringCASE(AUTO_PILOT);
	AgentModeFromStringCASE(MANUAL_PILOT);
	//Default to OFFLINE in case of error (as a asimov safety measure)
	return OFFLINE;
#undef AgentModeFromStringCASE
}




QDataStream &operator>>(QDataStream &ds, AgentMode &sm)
{
	quint32 mt=0;
	ds >> mt;
	sm=(AgentMode)mt;
	return ds;
}

QDataStream &operator<<(QDataStream &ds, const AgentMode &sm)
{
	quint32 mt=sm;
	ds << mt;
	return ds;
}


int bytes(const AgentMode &)
{
	return sizeof(qint32);
}

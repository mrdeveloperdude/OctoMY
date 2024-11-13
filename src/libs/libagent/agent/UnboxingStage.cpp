#include "UnboxingStage.hpp"

#include "uptime/MethodGate.hpp"

#include <QDebug>

QString UnboxingStageToSTring(UnboxingStage s)
{
	OC_FUNCTIONGATE();
#define  UnboxingStageToStringCASE(A) case (A):return #A
	switch(s) {
		UnboxingStageToStringCASE(DELIVERY_STAGE);
		UnboxingStageToStringCASE(CONTROLLER_TYPE_STAGE);
		UnboxingStageToStringCASE(CONTROLLER_CONFIGURATION_STAGE);
		UnboxingStageToStringCASE(STANZA_STAGE);
		UnboxingStageToStringCASE(PAIRING_STAGE);
		UnboxingStageToStringCASE(HANDOVER_STAGE);
		UnboxingStageToStringCASE(UNBOXING_COMPLETE);
		default:
		UnboxingStageToStringCASE(UNKNOWN_STAGE);
	}
#undef UnboxingStageToStringCASE
	return "UNKNOWN";
}

QDebug operator<<(QDebug debug, UnboxingStage s) {
	QDebugStateSaver saver(debug); // Saves the current debug state and restores it at the end
	debug.nospace() << UnboxingStageToSTring(s); // Calls your conversion function
	return debug;
}

#include "ControlUnboxingStage.hpp"

#include "uptime/MethodGate.hpp"

#include <QDebug>

QString ControlUnboxingStageToSTring(ControlUnboxingStage s)
{
	OC_FUNCTIONGATE();
#define  ControlUnboxingStageToStringCASE(A) case (A):return #A
	switch(s) {
		ControlUnboxingStageToStringCASE(DELIVERY_STAGE);
		ControlUnboxingStageToStringCASE(PAIRING_STAGE);
		ControlUnboxingStageToStringCASE(HANDOVER_STAGE);
		ControlUnboxingStageToStringCASE(UNBOXING_COMPLETE);
	default:
		ControlUnboxingStageToStringCASE(UNKNOWN_STAGE);
	}
#undef ControlUnboxingStageToStringCASE
	return "UNKNOWN";
}




QDebug operator<<(QDebug debug, ControlUnboxingStage s) {
	QDebugStateSaver saver(debug); // Saves the current debug state and restores it at the end
	debug.nospace() << ControlUnboxingStageToSTring(s); // Calls your conversion function
	return debug;
}


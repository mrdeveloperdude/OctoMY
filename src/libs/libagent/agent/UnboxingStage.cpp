#include "UnboxingStage.hpp"

#include "uptime/MethodGate.hpp"

QString UnboxingStageToSTring(UnboxingStage s)
{
	OC_FUNCTIONGATE();
#define  UnboxingStageToStringCASE(A) case (A):return #A
	switch(s) {
		UnboxingStageToStringCASE(CONTROLLER_CONFIGURATION_STAGE);
		UnboxingStageToStringCASE(CONTROLLER_TYPE_STAGE);
		UnboxingStageToStringCASE(DELIVERY_STAGE);
		UnboxingStageToStringCASE(PAIRING_STAGE);
		UnboxingStageToStringCASE(SERIAL_DEVICE_STAGE);
		UnboxingStageToStringCASE(ARDUMY_DEVICE_STAGE);
		UnboxingStageToStringCASE(HANDOVER_STAGE);
		UnboxingStageToStringCASE(UNBOXING_COMPLETE);
		default:
		UnboxingStageToStringCASE(UNKNOWN_STAGE);
	}
#undef UnboxingStageToStringCASE
	return "UNKNOWN";
}


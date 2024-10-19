#include "ControlUnboxingStage.hpp"


#include "uptime/MethodGate.hpp"

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


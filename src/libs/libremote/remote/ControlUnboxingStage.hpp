#ifndef CONTROLUNBOXINGSTAGE_HPP
#define CONTROLUNBOXINGSTAGE_HPP


#include <QString>

enum ControlUnboxingStage{
	  UNKNOWN_STAGE
	, DELIVERY_STAGE
	, PAIRING_STAGE
	, HANDOVER_STAGE
	, UNBOXING_COMPLETE
};

QString ControlUnboxingStageToSTring(ControlUnboxingStage s);

QDebug operator<<(QDebug debug, ControlUnboxingStage s);

#endif // CONTROLUNBOXINGSTAGE_HPP

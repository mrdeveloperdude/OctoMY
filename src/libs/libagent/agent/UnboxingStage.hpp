#ifndef UNBOXINGSTAGE_HPP
#define UNBOXINGSTAGE_HPP

#include <QString>

enum UnboxingStage{
	  UNKNOWN_STAGE
	, DELIVERY_STAGE
	, CONTROLLER_TYPE_STAGE
	, CONTROLLER_CONFIGURATION_STAGE
	, STANZA_STAGE
	, PAIRING_STAGE
	, HANDOVER_STAGE
	, UNBOXING_COMPLETE
};

QString UnboxingStageToSTring(UnboxingStage s);

QDebug operator<<(QDebug debug, UnboxingStage s);

#endif // UNBOXINGSTAGE_HPP

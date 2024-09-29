#include "IdenticonTooltip.hpp"

#include "security/PortableID.hpp"
#include "utility/string/String.hpp"

QString identiconTooltip(const PortableID &id){
	return QString("ID=%1\nNAME=%2\nTYPE=%3\nBIRTH=%4\n").arg(id.id()).arg(id.name()).arg(nodeTypeToString(id.type())).arg(utility::string::formattedDateFromMS(id.birthDate()));
}

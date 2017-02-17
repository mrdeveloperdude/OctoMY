#include "SerialSettings.hpp"

QString serialSettingsToString (const SerialSettings &set)
{
	QString out;
	out+="SerialSettings{name="+set.name;
	out+=", baudRate="+set.baudRate;
	out+=", dataBits="+set.dataBits;
	out+=", parity="+set.parity;
	out+=", stopBits="+set.stopBits;
	out+=", flowControl="+set.flowControl;
	out+="}";
	return out;
}

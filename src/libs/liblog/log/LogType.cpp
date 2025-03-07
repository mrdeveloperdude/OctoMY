#include "LogType.hpp"

#include "uptime/MethodGate.hpp"


QString logTypeToString(LogType logType)
{
	OC_FUNCTIONGATE();
	switch(static_cast<quint8>(logType)) {
#define LogType_case_stanza(A) case(A):{ return #A; }break
		LogType_case_stanza(LOG_TYPE_DEBUG);
		LogType_case_stanza(LOG_TYPE_INFO);
		LogType_case_stanza(LOG_TYPE_WARNING);
		LogType_case_stanza(LOG_TYPE_ERROR);
		LogType_case_stanza(LOG_TYPE_UNKNOWN);
	default: {
		return  QString("LOG_TYPE_UNKNOWN(%1)").arg(logType);
	}
#undef LogType_case_stanza
	}
}

QString logTypeToShortString(LogType logType){
	OC_FUNCTIONGATE();
	return QString(logTypeToString(logType)[10]);
}


LogType logTypeFromString(QString string)
{
	OC_FUNCTIONGATE();
#define LogType_if_stanza(A) if(#A == string){ return A; }
	LogType_if_stanza(LOG_TYPE_DEBUG);
	LogType_if_stanza(LOG_TYPE_INFO);
	LogType_if_stanza(LOG_TYPE_WARNING);
	LogType_if_stanza(LOG_TYPE_ERROR);
	LogType_if_stanza(LOG_TYPE_UNKNOWN);
#undef LogType_if_stanza
	return LOG_TYPE_UNKNOWN;
}


QDebug operator<< (QDebug d, LogType tt)
{
	OC_FUNCTIONGATE();
	QDebugStateSaver stateSaver(d);
	return d.noquote().nospace() << logTypeToString(tt);
}

#include "MessageType.hpp"


QDebug &operator<<(QDebug &d, const MessageType &type){
	switch(type){
		case(INVALID):{d.nospace() << "INVALID";}break;
		case(NOOP_MSG):{d.nospace() << "NOOP_MSG";}break;
		case(PING):{d.nospace() << "PING";}break;
		case(PONG):{d.nospace() << "PONG";}break;
		case(QUERY):{d.nospace() << "QUERY";}break;
		case(QUERY_RESULT):{d.nospace() << "QUERY_RESULT";}break;
		case(COMMAND):{d.nospace() << "COMMAND";}break;
		case(STATUS):{d.nospace() << "STATUS";}break;
		case(DIRECT_POSE):{d.nospace() << "DIRECT_POSE";}break;
		case(DISCOVERY_AUTH):{d.nospace() << "DISCOVERY_AUTH";}break;
		default: d.nospace() << "UNKNOWN";
	}
	return d.maybeSpace();
}


QString MessageTypeToString(MessageType s){
#define  MessageTypeToStringCASE(A) case (A):return #A
	switch(s){
		default:return "UNKNOWN";
			MessageTypeToStringCASE(INVALID);
			MessageTypeToStringCASE(NOOP_MSG);
			MessageTypeToStringCASE(PING);
			MessageTypeToStringCASE(PONG);
			MessageTypeToStringCASE(QUERY);
			MessageTypeToStringCASE(QUERY_RESULT);
			MessageTypeToStringCASE(COMMAND);
			MessageTypeToStringCASE(STATUS);
			MessageTypeToStringCASE(DIRECT_POSE);
			MessageTypeToStringCASE(DISCOVERY_AUTH);
	}
#undef MessageTypeToStringCASE
}



MessageType MessageTypeFromString(QString s){
	if("INVALID"==s)return INVALID;
	if("NOOP_MSG"==s)return NOOP_MSG;
	if("PING"==s)return PING;
	if("PONG"==s)return PONG;
	if("QUERY"==s)return QUERY;
	if("QUERY_RESULT"==s)return QUERY_RESULT;
	if("COMMAND"==s)return COMMAND;
	if("STATUS"==s)return STATUS;
	if("DIRECT_POSE"==s)return DIRECT_POSE;
	if("DISCOVERY_AUTH"==s)return DISCOVERY_AUTH;
	return INVALID;
}


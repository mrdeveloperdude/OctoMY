#include "MessageType.hpp"


QDebug &operator<<(QDebug &d, const MessageType &type)
{

#define  MessageTypeOperatorStreamLeftCASE(A) case (A):{d.nospace() << #A;}break

	switch(type) {
		MessageTypeOperatorStreamLeftCASE(INVALID);
		MessageTypeOperatorStreamLeftCASE(NOOP_MSG);
		MessageTypeOperatorStreamLeftCASE(PING);
		MessageTypeOperatorStreamLeftCASE(PONG);
		MessageTypeOperatorStreamLeftCASE(QUERY);
		MessageTypeOperatorStreamLeftCASE(QUERY_RESULT);
		MessageTypeOperatorStreamLeftCASE(COMMAND);
		MessageTypeOperatorStreamLeftCASE(STATUS);
		MessageTypeOperatorStreamLeftCASE(DIRECT_POSE);
		MessageTypeOperatorStreamLeftCASE(DISCOVERY_AUTH);
		MessageTypeOperatorStreamLeftCASE(AGENT_STATE);
	default:
		d.nospace() << "UNKNOWN";
	}
	return d.maybeSpace();
#undef MessageTypeOperatorStreamLeftCASE
}


QString MessageTypeToString(MessageType s)
{
#define  MessageTypeToStringCASE(A) case (A):return #A
	switch(s) {
	default:
		return "UNKNOWN";
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
		MessageTypeToStringCASE(AGENT_STATE);

	}
#undef MessageTypeToStringCASE
}



MessageType MessageTypeFromString(QString s)
{
#define  MessageTypeFromStringCASE(A) if(#A==s)return A
	MessageTypeFromStringCASE(INVALID);
	MessageTypeFromStringCASE(NOOP_MSG);
	MessageTypeFromStringCASE(PING);
	MessageTypeFromStringCASE(PONG);
	MessageTypeFromStringCASE(QUERY);
	MessageTypeFromStringCASE(QUERY_RESULT);
	MessageTypeFromStringCASE(COMMAND);
	MessageTypeFromStringCASE(STATUS);
	MessageTypeFromStringCASE(DIRECT_POSE);
	MessageTypeFromStringCASE(DISCOVERY_AUTH);
	MessageTypeFromStringCASE(AGENT_STATE);
	return INVALID;
#undef MessageTypeFromStringCASE
}

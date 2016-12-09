#include "MessageType.hpp"


QDebug &operator<<(QDebug &d, const MessageType &type)
{

#define  MessageTypeOperatorStreamLeftCASE(A) case (A):{d.nospace() << #A;}break

	switch(type) {
		MessageTypeOperatorStreamLeftCASE(INVALID);
		MessageTypeOperatorStreamLeftCASE(NOOP_MSG);
		MessageTypeOperatorStreamLeftCASE(PING);
		MessageTypeOperatorStreamLeftCASE(PONG);
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
	MessageTypeFromStringCASE(AGENT_STATE);
	return INVALID;
#undef MessageTypeFromStringCASE
}

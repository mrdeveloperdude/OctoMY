#include "MessageType.hpp"


QDebug &operator<<(QDebug &d, const MessageType &type)
{

#define  MessageTypeOperatorStreamLeftCASE(A) case (A):{d.nospace() << #A;}break

	switch(type) {
		MessageTypeOperatorStreamLeftCASE(INVALID);
		MessageTypeOperatorStreamLeftCASE(IDLE);
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
		MessageTypeToStringCASE(IDLE);
	}
#undef MessageTypeToStringCASE
}



MessageType MessageTypeFromString(QString s)
{
#define  MessageTypeFromStringCASE(A) if(#A==s)return A
	MessageTypeFromStringCASE(INVALID);
	MessageTypeFromStringCASE(IDLE);
	return INVALID;
#undef MessageTypeFromStringCASE
}

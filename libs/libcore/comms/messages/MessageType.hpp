#ifndef MESSAGETYPE
#define MESSAGETYPE

#include <QDebug>

enum MessageType{
	 INVALID=0
	, NOOP_MSG
	, PING
	, PONG
	, AGENT_STATE
};



QDebug &operator<<(QDebug &d, const MessageType &);



QString MessageTypeToString(MessageType s);


MessageType MessageTypeFromString(QString s);


#endif // MESSAGETYPE


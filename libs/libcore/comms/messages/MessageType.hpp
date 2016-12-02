#ifndef MESSAGETYPE
#define MESSAGETYPE

#include <QDebug>

enum MessageType{
	 INVALID=0
	, NOOP_MSG
	, PING
	, PONG
	, QUERY
	, QUERY_RESULT
	, COMMAND
	, STATUS
	, DIRECT_POSE
	, DISCOVERY_AUTH
	, AGENT_STATE
};



QDebug &operator<<(QDebug &d, const MessageType &);



QString MessageTypeToString(MessageType s);


MessageType MessageTypeFromString(QString s);


#endif // MESSAGETYPE


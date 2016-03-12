#include "QueryMessage.hpp"

#include <QDataStream>

QueryMessage::QueryMessage(
		const bool ok
		, const qint64 timestamp
		, const qint64 queryID
		, const qint64 deviceID
		, const QueryType queryType
		):
	ok(ok)
  , timestamp(timestamp)
  , queryID(queryID)
  , deviceID(deviceID)
  , queryType(queryType)
{
}


#include "MessageType.hpp"

QDataStream &operator>>(QDataStream &ds, QueryMessage &qm){
	qint32 mt=INVALID;
	ds >> mt;
	qm.ok=(QUERY==mt);
	if(qm.ok){
		ds >> qm.timestamp;
		ds >> qm.queryID;
		ds >> qm.deviceID;
		qint32  qt=NOOP;
		ds >> qt;
		qm.queryType=(QueryType)qt;
	}
	return ds;
}

QDataStream &operator<<(QDataStream &ds, const QueryMessage &qm){
	ds << QUERY;
	ds << qm.timestamp;
	ds << qm.queryID;
	ds << qm.deviceID;
	ds << qm.queryType;
	return ds;
}


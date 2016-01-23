#ifndef QUERYMESSAGE_HPP
#define QUERYMESSAGE_HPP

#include <QtGlobal>

#include "../QueryType.hpp"


class QueryMessage{
	public:
		bool ok;
		qint64 timestamp;
		qint64 queryID;
		qint64 deviceID;
		QueryType queryType;

	public:

		QueryMessage(
				const bool ok=false
				, const qint64 timestamp=0
				, const qint64 queryID=0
				, const qint64 deviceID=0
				, const QueryType queryType=NOOP
				);
};


QDataStream &operator<<(QDataStream &, const QueryMessage &);
QDataStream &operator>>(QDataStream &, QueryMessage &);

#endif // QUERYMESSAGE_HPP


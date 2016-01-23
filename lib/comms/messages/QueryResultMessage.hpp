#ifndef QUERYRESULTMESSAGE_HPP
#define QUERYRESULTMESSAGE_HPP

#include <QtGlobal>

#include "../DataType.hpp"


class QueryResultMessage{
	public:
		const qint64 timestamp;
		const qint64 queryID;
		const DataType dataType;
		const quint32 dataCount;
	public:
		QueryResultMessage(
				const qint64 timestamp=0
				, const qint64 queryID=0
				, const DataType dataType=NONE
				, const quint32 dataCount=0
				);

};

#endif // QUERYRESULTMESSAGE_HPP


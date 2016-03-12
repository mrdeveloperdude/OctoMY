#include "QueryResultMessage.hpp"


QueryResultMessage::QueryResultMessage(
		const qint64 timestamp
		, const qint64 queryID
		, const DataType dataType
		, const quint32 dataCount
		):
	timestamp(timestamp)
  , queryID(queryID)
  , dataType(dataType)
  , dataCount(dataCount)
{
}


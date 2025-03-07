#ifndef LOGTYPE_HPP
#define LOGTYPE_HPP



#include <QDebug>

enum LogType: quint8 {
	  LOG_TYPE_DEBUG
	, LOG_TYPE_INFO
	, LOG_TYPE_WARNING
	, LOG_TYPE_ERROR
	, LOG_TYPE_UNKNOWN
};


QString logTypeToString(LogType logType);
QString logTypeToShortString(LogType logType);
LogType logTypeFromString(QString string);

const QDebug &operator<<(QDebug &d, const LogType logType);


#endif // LOGTYPE_HPP

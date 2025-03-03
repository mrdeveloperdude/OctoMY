#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include <QDateTime>

class LogHandler
{
public:
	LogHandler();

public:
	static QString formatLogLine(QDateTime now, QtMsgType type, const QMessageLogContext &ctx, const QString &msg);
	static void octomyLogMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg);
	static void setLogging(bool log);

};

#endif // LOGHANDLER_H

#include "TestLogHandler.hpp"

#include "app/log/LogHandler.hpp"

#include <QDateTime>
#include <QDate>
#include <QTime>


void TestLogHandler::test()
{
	QMap<QtMsgType, QString> mts{
		{QtDebugMsg, "D: [13:37:42.069] test_loghandler - @fileName:1337 - functionName - TEST" }
		, {QtWarningMsg, "W: [13:37:42.069] test_loghandler - @fileName:1337 - functionName - TEST" }
		, {QtCriticalMsg, "C: [13:37:42.069] test_loghandler - @fileName:1337 - functionName - TEST" }
		, {QtFatalMsg, "F: [13:37:42.069] test_loghandler - @fileName:1337 - functionName - TEST" }
		, {QtInfoMsg, "I: [13:37:42.069] test_loghandler - @fileName:1337 - functionName - TEST" }
	};
	
	QMessageLogContext ctx("fileName", 1337, "functionName", "categoryName");
	
	const QString msg("TEST");
	const QDate d(2020, 2, 2);
	const QTime t(13, 37, 42, 69);
	const QDateTime dt(d, t);

	qDebug()<<"DATE:"<<d<<" TIME:"<<t<<" DATETIME:"<<dt;
	
	for(const auto &key:mts.keys()){
		const auto res = LogHandler::formatLogLine(dt, key, ctx, msg);
		QCOMPARE_EQ(res, mts[key]);
	}
}


OC_TEST_MAIN(test, TestLogHandler)


#include "LogHandler.hpp"


#include <QByteArray>
#include <QFile>
#include <QTextStream>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QPointer>
#include <QMutex>
#include <QMutexLocker>
#include <QFileInfo>
#include <QCoreApplication>




#include <iostream>



LogHandler::LogHandler()
{

}


static void octomyLogMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg){
	QString severity;
	//std::cerr<<"bob"<<std::endl;
	switch (type) {
		case QtDebugMsg:
			severity="D";
			break;
		case QtWarningMsg:
			severity="W";
			break;
		case QtCriticalMsg:
			severity="C";
			break;
		case QtFatalMsg:
			severity="F";
			break;
		case QtInfoMsg:
			severity="I";
			break;
	}
	const QDateTime now=QDateTime::currentDateTimeUtc();
	const QString line=QString::number(ctx.line);
	const QString file=QString::fromLocal8Bit(ctx.file);
	const QString fun=QString::fromLocal8Bit(ctx.function);
	const QString exe=QFileInfo( QCoreApplication::applicationFilePath()).fileName();
	QString out=QString("%1: [%2] %3 - @%5:%4 - %6 - %7")
			.arg(severity,now.toString("hh:mm:ss.zzz"), exe, line,file,fun, msg);
	if(true){
		std::cerr << out.toStdString() <<std::endl;
	}
	if(false){
		QFile outFile("log.txt");
		outFile.open(QIODevice::WriteOnly | QIODevice::Append);
		QTextStream ts(&outFile);
		ts << out << endl;
	}

}



void LogHandler::setLogging(bool log){
	qInstallMessageHandler(log?octomyLogMessageHandler:0);
}


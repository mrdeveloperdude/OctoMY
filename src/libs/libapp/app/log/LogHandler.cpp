#include "LogHandler.hpp"

#include <QString>
#include <QDateTime>
#include <QFileInfo>
#include <QCoreApplication>
#include <QTextStream>

#include <iostream>


LogHandler::LogHandler()
{

}


QString LogHandler::formatLogLine(QDateTime now, QtMsgType type, const QMessageLogContext &ctx, const QString &msg){
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
	const QString line = QString::number(ctx.line);
	const QString file = QString::fromLocal8Bit(ctx.file);
	const QString fun = QString::fromLocal8Bit(ctx.function);
	//std::cout<<"AAAA\n";
	const QString exe = QFileInfo( QCoreApplication::applicationFilePath()).fileName();
	//std::cout<<"BBBB\n";
	bool filter = false;
	if(msg.startsWith("OC_RECUSTION_BREAKER")){
		filter = true;
	}
	if(msg == "link  hasn't been detected!") {
		//filter = true;
	}
	if(msg.contains("hasn't been detected!")) {
		filter = true;
	}
	if(msg.contains("Cannot queue argument")) {
		//filter=true;
	}
	QString out;
	if(!filter) {
		if(msg.startsWith("   Loc: [")) {
			out = msg;
		}
		else{
			out = QString("%1: [%2] %3 - @%5:%4 - %6 - %7").arg(severity, now.toString("hh:mm:ss.zzz"), exe, line, file, fun, msg);
		}
	}
	return out;
}


void LogHandler::octomyLogMessageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
	const QDateTime now = QDateTime::currentDateTimeUtc();
	QStringList list = msg.split("[\n\r]", Qt::SkipEmptyParts);
	QString out;
	auto &output = type == QtWarningMsg || type == QtCriticalMsg || type == QtFatalMsg? std::cerr:std::cout;
	for(auto &str:list) {
		auto line = LogHandler::formatLogLine(now, type, ctx, str).toStdString();
		if(line!=""){
			output << line << std::endl;
		}
	}
	if(false) {
		QFile outFile("log.txt");
		outFile.open(QIODevice::WriteOnly | QIODevice::Append);
		QTextStream ts(&outFile);
		ts << out << "\n";
	}
}



void LogHandler::setLogging(bool log)
{
	qInstallMessageHandler(log?LogHandler::octomyLogMessageHandler:nullptr);
}

#include "Utilities.hpp"

#include "utility/Utility.hpp"

#include <QDateTime>
#include <QCoreApplication>
#include <QTest>

void testSleep(quint64 ms, QString occation)
{
	if(!occation.isEmpty()) {
		qDebug()<<"Sleeping, waiting for "+occation;
	}
	const quint64 start=utility::currentMsecsSinceEpoch<quint64>();
	quint64 now=start;
	while(now<start+ms) {
		now=utility::currentMsecsSinceEpoch<quint64>();
		QCoreApplication::processEvents();
	}
	if(!occation.isEmpty()) {
		qDebug()<<"Sleeping over for "+occation;
	}

}





void testHeading(QString msg, QString ch)
{
	auto sz=msg.size();
	auto full=80-4;
	auto half=(full-sz)/2;
	auto second=full-sz-half;
	QTest::waitForEvents();
	qDebug()<<"";
	qDebug()<<"";
	qDebug().nospace().noquote()<<QString(ch).repeated(half)<< " # "<<msg << " # "<<QString(ch).repeated(second);
	qDebug()<<"";
}

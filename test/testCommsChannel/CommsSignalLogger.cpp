#include "CommsSignalLogger.hpp"


CommsSignalLogger::CommsSignalLogger(QString name, QObject *parent)
	: QObject(parent)
	, mName(name)
{

}
CommsSignalLogger::~CommsSignalLogger()
{

}


void CommsSignalLogger::onCommsError(QString message)
{
	qDebug()<<mName<<": onCommsError("<<message<<")";
}

void CommsSignalLogger::onCommsClientAdded(CommsSession *c)
{
	qDebug()<<mName<<": onCommsClientAdded("<<c<<")";
}

void CommsSignalLogger::onCommsConnectionStatusChanged(bool c)
{
	qDebug()<<mName<<": onCommsConnectionStatusChanged("<<c<<")";
}



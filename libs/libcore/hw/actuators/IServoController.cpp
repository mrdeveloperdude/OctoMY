#include "IServoController.hpp"


IServoController::IServoController(QString name, QObject *parent)
	: QObject(parent)
	, mName(name)
{

}

IServoController::~IServoController()
{

}

QString IServoController::controllerName()
{
	return mName;
}

QString IServoController::controllerVersion()
{
	return "Unknown version";
}

bool IServoController::isDebug()
{
	return false; //TODO: Implement
}

void IServoController::setDebug(bool)
{
//TODO: Implement
}


void IServoController::setServosCount(quint32 ct)
{

}


void IServoController::killAll()
{
//TODO: Implement
}

void IServoController::centerAll()
{
//TODO: Implement
}


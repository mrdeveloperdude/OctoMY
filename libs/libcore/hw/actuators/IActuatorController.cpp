#include "IActuatorController.hpp"

#include <QWidget>
#include <QBitArray>

IActuatorController::IActuatorController(QString name, QObject *parent)
	: QObject(parent)
	, mName(name)
{

}

IActuatorController::~IActuatorController()
{

}

QString IActuatorController::controllerName()
{
	return mName;
}

QString IActuatorController::controllerVersion()
{
	return "Unknown version";
}

bool IActuatorController::isDebug()
{
	return false; //TODO: Implement
}

void IActuatorController::setDebug(bool)
{
//TODO: Implement
}


void IActuatorController::limpAll()
{
	const auto count=actuatorCount();
	QBitArray ar(count,true);
	limp(ar);
}

void IActuatorController::centerAll()
{
	const auto count=actuatorCount();
	for(quint8 i=0; i<count; ++i) {
		move(i, actuatorDefault(i));
	}
}



QWidget *IActuatorController::configurationWidget()
{
	return nullptr;
}



QVariantMap IActuatorController::confiruation()
{
	QVariantMap map;
	return map;

}

void IActuatorController::setConfiguration(QVariantMap &)
{

}

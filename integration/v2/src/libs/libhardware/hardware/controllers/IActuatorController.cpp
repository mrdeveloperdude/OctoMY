#include "IActuatorController.hpp"

#include "uptime/MethodGate.hpp"

#include <QWidget>
#include <QBitArray>

IActuatorController::IActuatorController(QString name, QObject *parent)
	: QObject(parent)
	, mName(name)
{
	OC_METHODGATE();
}

IActuatorController::~IActuatorController()
{
	OC_METHODGATE();

}

QString IActuatorController::controllerName()
{
	OC_METHODGATE();
	return mName;
}

QString IActuatorController::controllerVersion()
{
	OC_METHODGATE();
	return "Unknown version";
}

bool IActuatorController::isDebug()
{
	OC_METHODGATE();
	return false; //TODO: Implement
}

void IActuatorController::setDebug(bool)
{
	OC_METHODGATE();
//TODO: Implement
}


void IActuatorController::limpAll()
{
	OC_METHODGATE();
	const auto count=actuatorCount();
	QBitArray ar(count,true);
	limp(ar);
}

void IActuatorController::centerAll()
{
	OC_METHODGATE();
	const auto count=actuatorCount();
	for(quint8 i=0; i<count; ++i) {
		move(i, actuatorDefault(i));
	}
}



QWidget *IActuatorController::configurationWidget()
{
	OC_METHODGATE();
	return nullptr;
}



QVariantMap IActuatorController::configuration()
{
	OC_METHODGATE();
	QVariantMap map;
	return map;

}

void IActuatorController::setConfiguration(QVariantMap &)
{
	OC_METHODGATE();
}

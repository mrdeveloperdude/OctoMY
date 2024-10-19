#include "IController.hpp"

#include "uptime/MethodGate.hpp"

#include <QWidget>
#include <QBitArray>

IController::IController(QString title, QString description, QString icon, QObject *parent)
	: QObject(parent)
	, mTitle(title)
	, mDescription(description)
	, mIcon(icon)
{
	OC_METHODGATE();
}

IController::~IController()
{
	OC_METHODGATE();
}

// IController wrapping
//////////////////////////////////////////////


QString IController::controllerTitle() const
{
	OC_METHODGATE();
	return mTitle;
}

QString IController::controllerDescription() const
{
	OC_METHODGATE();
	return mDescription;
}

QString IController::controllerIcon() const
{
	OC_METHODGATE();
	return mIcon;
}

QString IController::controllerVersion()
{
	OC_METHODGATE();
	return "Unknown version";
}

bool IController::isDebug()
{
	OC_METHODGATE();
	return mDebug;
}

void IController::setDebug(bool doDebug)
{
	OC_METHODGATE();
	mDebug = doDebug;
}




// IController interface
//////////////////////////////////////////////


bool IController::hasConfigurationWidget() const{
	OC_METHODGATE();
	return false;
}


QWidget *IController::configurationWidget()
{
	OC_METHODGATE();
	return nullptr;
}

QVariantMap IController::configuration()
{
	OC_METHODGATE();
	return mSettings;
}


void IController::setConfiguration(const QVariantMap &map)
{
	OC_METHODGATE();
	mSettings = map;
}


void IController::limp(const QBitArray &flags){
	OC_METHODGATE();
	if(isConnected()) {
		const int sz = flags.size();
		// Trivial reject: limp ALL
		if(flags.count(true)==sz) {
			limpAll();
		} else {
			QString data;
			for(auto i = 0; i < sz; ++i) {
				limp(i, flags.testBit(i));
			}
		}
	} else {
		qWarning() << "ERROR: Trying to limp subset of actuators when not connected";
	}
}


void IController::move(const Pose &pose){
	OC_METHODGATE();
	if(isConnected()) {
		const auto poseSize{static_cast<int>(pose.size())};
		const auto actuatorSize = actuatorCount();
		if(poseSize>actuatorSize){
			qWarning() << "Pose size "<< poseSize << " exceeds available number of actuators " << actuatorSize << ", upper pose values will be ignoderd";
		}
		for(int i = 0; i < actuatorSize; ++i) {
			// TODO: Fix float vs int issue
			const auto value = qBound(-1.0, pose.value(i), 1.0);
			move(i, value);
		}
	} else {
		qWarning() << "ERROR: Trying to move actuators when not connected";
	}
}


void IController::center(quint8 index){
	OC_METHODGATE();
	move(index, actuatorDefault(index));
}


void IController::limpAll()
{
	OC_METHODGATE();
	const auto count=actuatorCount();
	QBitArray ar(count,true);
	limp(ar);
}


void IController::centerAll()
{
	OC_METHODGATE();
	const auto count=actuatorCount();
	for(quint8 i=0; i<count; ++i) {
		move(i, actuatorDefault(i));
	}
}


QString IController::debugString(){
	OC_METHODGATE();
	return "No Debug";
}

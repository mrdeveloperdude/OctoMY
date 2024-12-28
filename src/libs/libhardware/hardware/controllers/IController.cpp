#include "IController.hpp"

#include "uptime/MethodGate.hpp"

#include <QWidget>
#include <QBitArray>

IController::IController(const QString &title, const QString &description, const QString &icon, QObject *parent)
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


QString IController::firmwareVersion(){
	return "NOT IMPLEMENTED";
}


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


void IController::setEnabled(ACTUATOR_INDEX index, bool enabled){
	OC_METHODGATE();
	if(mEnabled.size()<=index) {
		mEnabled.resize(index+1);
	}
	mEnabled[index] = enabled;
}


bool IController::isEnabled(ACTUATOR_INDEX index) const {
	OC_METHODGATE();
	if(mEnabled.size()<=index) {
		return false;
	}
	return mEnabled[index];
}


ACTUATOR_INDEX IController::maxActuatorsSupported(){
	OC_METHODGATE();
	return 0;
}


ACTUATOR_INDEX IController::actuatorCount(){
	OC_METHODGATE();
	return 0;
}


QString IController::actuatorName(ACTUATOR_INDEX index){
	Q_UNUSED(index);
	OC_METHODGATE();
	return "NOT IMPLEMENTED";
}


ACTUATOR_VALUE IController::actuatorTargetValue(ACTUATOR_INDEX index){
	Q_UNUSED(index);
	OC_METHODGATE();
	return 0;
}


ACTUATOR_VALUE IController::actuatorDefaultValue(ACTUATOR_INDEX index){
	Q_UNUSED(index);
	OC_METHODGATE();
	return 0;
}


ActuatorMotion IController::actuatorMotion(ACTUATOR_INDEX index){
	OC_METHODGATE();
	Q_UNUSED(index);
	return RANGED_MOTION;
}


bool IController::isLimp(ACTUATOR_INDEX index){
	Q_UNUSED(index);
	OC_METHODGATE();
	return true;
}


void IController::setLimp(ACTUATOR_INDEX index, bool limp){
	Q_UNUSED(index);
	Q_UNUSED(limp);
	OC_METHODGATE();
}


void IController::setTargetValue(ACTUATOR_INDEX index, ACTUATOR_VALUE value){
	Q_UNUSED(index);
	Q_UNUSED(value);
	OC_METHODGATE();
}


void IController::setLimp(const QBitArray &flags){
	OC_METHODGATE();
	if(isConnected()) {
		const int sz = flags.size();
		// Trivial reject: limp ALL
		if(flags.count(true) == sz) {
			toggleLimpAll(true);
		} else if(flags.count(false) == sz) {
			toggleLimpAll(false);
		} else {
			for(auto i = 0; i < sz; ++i) {
				setLimp(i, flags.testBit(i));
			}
		}
	} else {
		qWarning() << "ERROR: Trying to limp subset of actuators when not connected";
	}
}


void IController::setTargetPose(const Pose &pose){
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
			setTargetValue(i, value);
		}
	} else {
		qWarning() << "ERROR: Trying to move actuators when not connected";
	}
}


void IController::center(ACTUATOR_INDEX index){
	OC_METHODGATE();
	setTargetValue(index, actuatorDefaultValue(index));
}


void IController::toggleLimpAll(bool limp)
{
	OC_METHODGATE();
	const auto count = actuatorCount();
	QBitArray ar(count, limp);
	setLimp(ar);
}


void IController::centerAll()
{
	OC_METHODGATE();
	const auto count=actuatorCount();
	for(ACTUATOR_INDEX i=0; i<count; ++i) {
		setTargetValue(i, actuatorDefaultValue(i));
	}
}


QString IController::debugString(){
	OC_METHODGATE();
	return "No Debug";
}


// Management of sensor definitions
//////////////////////////////////////////////


ACTUATOR_INDEX IController::maxSensorsSupported(){
	OC_METHODGATE();
	return 0;
}


ACTUATOR_INDEX IController::sensorCount(){
	OC_METHODGATE();
	return 0;
}


QString IController::sensorName(ACTUATOR_INDEX index){
	Q_UNUSED(index);
	OC_METHODGATE();
	return "";
}


// Management of sensor state
//////////////////////////////////////////////


ACTUATOR_VALUE IController::sensorValue(ACTUATOR_INDEX index){
	Q_UNUSED(index);
	OC_METHODGATE();
	return 0;
}


// Management of lobe definitions
//////////////////////////////////////////////


bool IController::lobesSupported(){
	OC_METHODGATE();
	return false;
}


bool IController::setLobe(ACTUATOR_INDEX index, const QByteArray &lobeData){
	Q_UNUSED(index);
	Q_UNUSED(lobeData);
	OC_METHODGATE();
	return false;
}


QString IController::getLobeTitle(ACTUATOR_INDEX index){
	Q_UNUSED(index);
	OC_METHODGATE();
	return "";
}


QString IController::getLobeVersion(ACTUATOR_INDEX index){
	Q_UNUSED(index);
	OC_METHODGATE();
	return "";
}


QString IController::getLobeID(ACTUATOR_INDEX index){
	Q_UNUSED(index);
	OC_METHODGATE();
	return "";
}


// Management of lobe state
//////////////////////////////////////////////


void IController::getLobeEnabled(ACTUATOR_INDEX index, bool enabled){
	Q_UNUSED(index);
	Q_UNUSED(enabled);
	OC_METHODGATE();
}






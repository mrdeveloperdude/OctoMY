#include "ArduMYController.hpp"

#include "hw/serial/SerialSettingsWidget.hpp"
#include "../arduino/ArduMYParserState.hpp"
#include "utility/Standard.hpp"
#include "ArduMYControllerWidget.hpp"
#include "ArduMYTypeConversions.hpp"

#include <QBuffer>
#include <QDebug>


ArduMYControllerWidget *mWidget;


ArduMYController::ArduMYController(QObject *parent)
	: IActuatorController("ArduMY", parent)
	, mSerialInterface(new QSerialPort(this))
	, mCommandSerializer(mActuators)
	, mLimpDirty(true)
	, mCountDirty(true)
	, mSyncDirty(true)
	, mWidget(nullptr)
{

	qRegisterMetaType<QSerialPort::SerialPortError>();
	if(!connect(mSerialInterface, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(onSerialHandleError(QSerialPort::SerialPortError)), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(mSerialInterface, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(onSerialHandleError(QSerialPort::SerialPortError)), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(mSerialInterface, SIGNAL(readyRead()), this, SLOT(onSerialReadData()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	if(!connect(mSerialInterface, SIGNAL(bytesWritten(qint64)), this, SLOT(onSerialDataWritten(qint64)), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	/*
	if(!connect(&mSerialSettings,SIGNAL(settingsChanged()), this, SLOT(onSerialSettingsChanged()), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
	*/

	mSyncTimer.setTimerType(Qt::VeryCoarseTimer);
	mSyncTimer.setInterval(1000);

	if(!connect(&mSyncTimer,&QTimer::timeout, this, &ArduMYController::onSendingTimer, OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
}

ArduMYController::~ArduMYController()
{
	//ASIMOV: Limp all servos before closing shop to avoid frying them if they are trying to reach impossible positions
	limpAll();
}


void ArduMYController::setSerialConfig(SerialSettings settings)
{
	qDebug()<<"SERIAL SETTINGS SET FOR ARDUMY: "<<serialSettingsToString(settings);
	mSerialSettings=settings;
}


SerialSettings &ArduMYController::serialSettings()
{
	return mSerialSettings;
}



void ArduMYController::openSerialPort()
{
	if(mSerialInterface->isOpen()) {
		qWarning()<<"ERROR OPENING ARDUMY: ALREADY OPEN";
		return;
	}
	mSerialInterface->setPortName(mSerialSettings.name);
	mSerialInterface->setBaudRate(mSerialSettings.baudRate);
	mSerialInterface->setDataBits(mSerialSettings.dataBits);
	mSerialInterface->setParity(mSerialSettings.parity);
	mSerialInterface->setStopBits(mSerialSettings.stopBits);
	mSerialInterface->setFlowControl(mSerialSettings.flowControl);
	qDebug()<<tr("Trying to connect to ardumy with %1 : %2, %3, %4, %5, %6").arg(mSerialSettings.name).arg(mSerialSettings.stringBaudRate()).arg(mSerialSettings.stringDataBits()).arg(mSerialSettings.stringParity()).arg(mSerialSettings.stringStopBits()).arg(mSerialSettings.stringFlowControl());
	if (mSerialInterface->open(QIODevice::ReadWrite)) {
		qDebug()<<"ARDUMY CONNECT SUCCESSFULL";
		mSyncTimer.start();
	} else {
		qWarning()<<"ERROR OPENING: "<<mSerialInterface->errorString();
	}
	emit connectionChanged();
}

void ArduMYController::closeSerialPort()
{
	mSyncTimer.stop();
	if (mSerialInterface->isOpen()) {
		mSerialInterface->close();
		emit connectionChanged();
		qDebug()<<"ARDUMY DISCONNECT SUCCESSFULL";
	}
}


void ArduMYController::onSendingTimer()
{
	//qDebug()<<"ON SENDING TIMER";
	syncData();
}


// NOTE: This will carry out the actual writing when sending timer signals there is an opportunity.
//	     The data for move is accumulated and consolidated by one or more move() commands.
void ArduMYController::syncData()
{
	if(isConnected()) {
		if(OCTOMY_AWAITING_COMMAND!=mCommandSerializer.currentCommand) {
			qDebug()<<"----- ARDUMY SERIALIZER STATUS: "<< ardumyCommandSerializerToString(mCommandSerializer);
		}
		QByteArray ba;
		while(mCommandSerializer.hasMoreData()) {
			uint8_t byte=mCommandSerializer.nextByte();
			ba.append(byte);
		}
		int sz=ba.size();
		if(sz>0) {
			writeData(ba);
			qDebug()<<"ARDUMY SERIAL WROTE "<<sz<<" bytes to serial";
		} else {
			//qDebug()<<"ARDUMY SERIAL WROTE NO DATA";
		}

	} else {
		qWarning()<<"ERROR: Trying to syncMove with serial when not connected";
	}
}


void ArduMYController::writeData(const QByteArray &data)
{
	if(0==data.size()) {
		qWarning()<<"ERROR: WRITING CALLED WITH NO DATA";
	} else {
		const qint64 accepted=mSerialInterface->write(data);
		if(accepted<0) {
			qWarning()<<"ERROR: write was not accepted. ERROR="<<mSerialInterface->errorString();
		} else {
			if(data.size()>accepted) {
				qWarning()<<"ERROR: Data truncation occurred, serial port would not accept more";
			}
			//serial->flush();
			//qDebug()<<"WRITING: "<<data.size()<<" accepted "<<accepted;
		}
	}
}

// Actuator Widget slots
//////////////////////////////////////////////



void ArduMYController::onActuatorWidgetMoved(quint32 id, qreal val)
{
	//qDebug()<<"ARDUMY ACTUATOR WIDGET MOVE"<<id<<" TO "<<val;
	move(id,val);
}

void ArduMYController::onActuatorWidgetLimped(quint32 id, bool l)
{
	qDebug()<<"ARDUMY ACTUATOR WIDGET LIMP "<<id<<" TO "<<l;
	limp(id,l);
}

void ArduMYController::onActuatorWidgetDeleted(quint32 id)
{
	qDebug()<<"ARDUMY ACTUATOR WIDGET DELETE"<<id;
	deleteActuator(id);
}



// Serial IO slots
//////////////////////////////////////////////

void ArduMYController::onSerialReadData()
{
	QByteArray data=mSerialInterface->readAll();
	qDebug()<<"ARDUMY SERIAL READ "<<data.size()<<" bytes:";
	qDebug()<<data;
}

void ArduMYController::onSerialDataWritten(qint64 b)
{
	const qint64 left=mSerialInterface->bytesToWrite();
	qDebug()<<"ARDUMY SERIAL WRITE "<<b<<", left="<<left;
	if(left<=0 && mCommandSerializer.hasMoreData()) {
		syncData();
	}
}

void ArduMYController::onSerialHandleError(QSerialPort::SerialPortError error)
{
	if (error != QSerialPort::NoError) {
		qDebug()<<"ARDUMY SERIAL ERROR "<<error<<": "<<mSerialInterface->errorString();
	}
	if (error == QSerialPort::ResourceError) {
		qDebug()<<"Critical Error "<<mSerialInterface->errorString();
		closeSerialPort();
	}

}

void ArduMYController::onSerialSettingsChanged()
{
	{
		QSignalBlocker(this);
		closeSerialPort();
		openSerialPort();
	}
	emit settingsChanged();
}


// IServoController interface
//////////////////////////////////////////////


void ArduMYController::setConnected(bool open)
{
	if(open) {
		openSerialPort();
		qDebug()<<"Opened ArduMY controller version "<<version();
	} else {
		closeSerialPort();
	}
}

bool ArduMYController::isConnected()
{
	return mSerialInterface->isOpen();
}


ArduMYActuator *ArduMYController::addActuator()
{
	ArduMYActuator *ret=nullptr;
	const quint8 max=maxActuatorsSupported();
	const auto ct=mActuators.size();
	if(ct==max) {
		qWarning()<<"ERROR: Tried to add actuator beyond the maximum count of " <<max;
	} else {
		mActuators.setSize(ct+1);
		mCountDirty=true;
		emit actuatorConfigurationChanged();
	}
	ret=&mActuators[ct];
	return ret;
}

void ArduMYController::deleteActuator(quint32 id)
{
	qDebug()<<"SIZE BEFORE DELETE: "<<mActuators.size();
	mActuators.remove(id);
	qDebug()<<"SIZE AFTER DELETE: "<<mActuators.size();
	emit actuatorConfigurationChanged();
}

void ArduMYController::setActuatorCount(quint8 ct)
{
	const quint8 max=maxActuatorsSupported();
	if(ct>max) {
		qWarning()<<"ERROR: Tried to set "<<ct<< " actuators which is more than the maximum of " <<max;
	} else {
		const quint8 old=mActuators.size();
		if(ct!=old) {
			mActuators.setSize(ct);
			if(ct>old) {
				//TODO: Initialize new ones?
			}
			mCountDirty=true;
		}
	}
}

ArduMYActuatorSet &ArduMYController::actuators()
{
	return mActuators;
}


void ArduMYController::limp(QBitArray &flags)
{
	if(isConnected()) {
		//for(ArduMYActuator &actuator:mActuators) {
		for(uint32_t e=mActuators.size(),i=0; i<e; ++i) {
			ArduMYActuator &actuator=mActuators[i];
			const bool k=flags.testBit(i);
			if(k!=actuator.state.isLimp()) {
				mLimpDirty=true;
				actuator.state.setLimp(k);
			}
		}
	} else {
		qWarning()<<"ERROR: Trying to limp subset of actuators via serial when not connected";
	}
}

void ArduMYController::limp(quint8 index, bool limp)
{
	if(isConnected()) {
		if(index>mActuators.size()) {
			return;
		}
		ArduMYActuator &actuator=mActuators[index];
		if(limp!=actuator.state.isLimp()) {
			mLimpDirty=true;
			actuator.state.setLimp(limp);
		}
	} else {
		qWarning()<<"ERROR: Trying to limp single atuator via serial when not connected";
	}
}

// NOTE: This will simply collect the latest data. The actual writing is done in syncMove when serial signals there is an opportunity.
// TODO: look at binary extension introduced in 2.1 version of hexy firmware to improve performance
void ArduMYController::move(Pose &pose)
{
	//qreal pos[1]= {0.0};	quint32 flags;
	if(isConnected()) {
		/*
		const quint32 sz=pose.size();
		if(mAccumulatedPosition.size()<sz) {
			mAccumulatedPosition.resize(sz);
			mDirtyMoveFlags.resize(sz);
		}
		for(quint32 i=0; i<sz; ++i) {
			const quint32 p=(quint32)(qBound(-1.0, pose.value(i), 1.0)*1000.0+1500.0);
			//Skip unecessary communication if value did not change
			if(mAccumulatedPosition[i]!=p) {
				mAccumulatedPosition[i]=p;
				mDirtyMoveFlags.setBit(i);
			}
		}
		//qDebug()<<"MOVE: flags="<< QString("%1").arg( flags, 16, 2, QChar('0'))<< ", dirtyMoveFlags="<<QString("%1").arg( dirtyMoveFlags, 16, 2, QChar('0'));
		if(mDirtyMoveFlags.count(true)>0 && 0==mSerialInterface->bytesToWrite()) {
			syncData();
		}
		*/
	} else {
		qWarning()<<"ERROR: Trying to move actuators with serial when not connected";
	}
}



void ArduMYController::move(quint8 i, qreal value)
{
	//qDebug()<<"ARDUMY MOVE: "<<i<<value;
	/*
	const quint32 p=(quint32)(qBound(-1.0, value, 1.0)*1000.0+1500.0);
	//Skip unecessary communication if value did not change
	if(mAccumulatedPosition[i]!=p) {
		mAccumulatedPosition[i]=p;
		if(0==mSerialInterface->bytesToWrite()) {
			syncMove();
		}
	}
	*/
}


QString ArduMYController::version()
{
	return "VERSION: IMPLEMENT ME";
}



quint8 ArduMYController::maxActuatorsSupported()
{
	return 0xFF;
}


quint8 ArduMYController::actuatorCount()
{
	return mActuators.size();
}

QString ArduMYController::actuatorName(quint8)
{
	return "IMPLEMENT ME";
}

qreal ArduMYController::actuatorValue(quint8 index)
{
	if(index>=mActuators.size()) {
		return 0.0f;
	}
	return mActuators[index].state.value.toFloat(mActuators[index].config.representation);
}


qreal ArduMYController::actuatorDefault(quint8)
{
	return 0.0f;
}


QWidget *ArduMYController::configurationWidget()
{
	if(nullptr==mWidget) {
		mWidget=new ArduMYControllerWidget(nullptr);
		if(nullptr!=mWidget) {
			mWidget->configure(this);
		}
	}
	return mWidget;
}



QVariantMap ArduMYController::configuration()
{
	QVariantMap map;
	map["version"]="ArduMY™ v0.1b";
	{
		QVariantList actuatorList;
		const uint32_t act=mActuators.size();
		for(uint32_t aix=0; aix<act; ++aix) {
			ArduMYActuatorValueRepresentation representation;
			ArduMYActuator &actuator=mActuators[aix];
			QVariantMap actuatorMap;
			actuatorMap["index"]=aix;
			{

				QVariantMap configMap;
				ArduMYActuatorConfig &config=actuator.config;
				representation=config.representation;
				configMap["flags"]=config.flags;
				/*
				  ARDUMY_ACTUATOR_FLAG_SELECTOR(isLinear,				setLinear,				0 ) // Linear as opposed to rotary means that the actuator acts in a straigt motion similar to a hydraulic cylinder instead of a rotary motion similar to an RC servo
				  ARDUMY_ACTUATOR_FLAG_SELECTOR(isContinuous,			setContinuous,			1 ) // Continuous as opposed to ranged means that the actuator continues running without any stop point, as a motor instead of a servo
				  ARDUMY_ACTUATOR_FLAG_SELECTOR(hasGearRatio,			setGearRatio,			2 ) // Gear ratio means that the motor shaft may turn more than one turn per output shaft turn.
				  ARDUMY_ACTUATOR_FLAG_SELECTOR(hasPositionFeedback,		setPositionFeedback,	3 ) // Position feedback means that the actuator reports its's position from an sensor such as potensiometer to an analogue input pin
				  ARDUMY_ACTUATOR_FLAG_SELECTOR(hasTachometer,			setTachometer,			4 ) // Tachometer means that the actuator reports it's speed from a sensor such as a hall effect sensor to an digital input pin
				  ARDUMY_ACTUATOR_FLAG_SELECTOR(hasIncrementalEncoder,	setIncrementalEncoder,	5 ) // Incremental encoder means the actuator reports it's  position from an encoder via 2 digital pins. NOTE: The encoding algorithm should take gear-ratio into account
				  ARDUMY_ACTUATOR_FLAG_SELECTOR(hasAbsoluteEncoder,		setAbsoluteEncoder,		6 ) // NOTE: NOT IMPLEMENTED! Absolute encoder means the actuator reports it's  position from an encoder via X digital pins, where X usually is in the 2-16 range. NOTE: The encoding algorithm should take gear-ratio into account
				  ARDUMY_ACTUATOR_FLAG_SELECTOR(hasLimitSwitchStart,		setLimitSwitchStart,	7 ) // Limit switch start means the actuator has a limit switch telling the softare when it has reached the beginning of its range. This can be used as a security measure as well as a means for automatical reference calibration. The limit state is reported via a digital input pin
				  ARDUMY_ACTUATOR_FLAG_SELECTOR(hasLimitSwitchEnd,		setLimitSwitchEnd,		8 ) // Limit switch end means the actuator has a limit switch telling the softare when it has reached the end of its range. This can be used as a security measure as well as a means for automatical reference calibration. The limit state is reported via a digital input pin
				  ARDUMY_ACTUATOR_FLAG_SELECTOR(isDirty,					setDirty,				9 ) // Dirty means the configuration has changed and must be updated at opportunity. NOTE: This bit should be ignored when data is serialized, as it representes ephemeral state. TODO: Look at storing this outside of this class
					*/

				configMap["nickName"]=ardumyActuatorNameToString(config);
				configMap["type"]=ardumyActuatorTypeToString(config.type);
				configMap["representation"]=ardumyActuatorValueRepresentationToString(representation);

				//QString("%1").arg(mID, 2, 10, QChar('0'));
				configMap["gearRatioNumerator"]=config.gearRatioNumerator;
				configMap["gearRatioDenominator"]=config.gearRatioDenominator;
				configMap["positionFeedbackPin"]=config.positionFeedbackPin;
				configMap["tachometerPin"]=config.tachometerPin;
				configMap["incrementalEncoderPinA"]=config.incrementalEncoderPinA;
				configMap["incrementalEncoderPinB"]=config.incrementalEncoderPinB;
				configMap["incrementalencoderDebounceCount"]=config.incrementalencoderDebounceCount;
				configMap["limitSwitchPinStart"]=config.limitSwitchPinStart;
				configMap["limitSwitchPinEnd"]=config.limitSwitchPinEnd;
				configMap["limitSwitchDebounceCount"]=config.limitSwitchDebounceCount;
				configMap["incrementalencoderDebounceCount"]=config.incrementalencoderDebounceCount;
				configMap["stepMotorPhaseCount"]=config.stepMotorPhaseCount;
				configMap["stepMotorStepsPerRotation"]=config.stepMotorStepsPerRotation;
				configMap["rcServoPin"]=config.rcServoPin;
				configMap["rcServoPin"]=config.rcServoPin;
				configMap["rangeStart"]=ardumyActuatorValueToVariant(config.rangeStart, config.representation);
				configMap["rangeSpan"]=ardumyActuatorValueToVariant(config.rangeSpan, config.representation);
				actuatorMap["config"]=configMap;
			}
			{
				QVariantMap stateMap;
				ArduMYActuatorState &state=actuator.state;
				stateMap["flags"]=state.flags;
				/*
				ARDUMY_ACTUATOR_FLAG_SELECTOR(isLimp,					setLimp,				0 )
				ARDUMY_ACTUATOR_FLAG_SELECTOR(isDirty,					setDirty,				1 )
				*/
				stateMap["value"]=ardumyActuatorValueToVariant(state.value, representation);
				actuatorMap["state"]=stateMap;
			}
			actuatorList<<actuatorMap;
		}
		map["actuators"]=actuatorList;
	}
	{
		map["serial"]=mSerialSettings.toMap();
	}

	return map;

}

void ArduMYController::setConfiguration(QVariantMap &configuration)
{
	qDebug()<<"Loading ArduMY configuration version '"<<configuration["version"]<<"'";
	if(configuration.contains("actuators")) {
		QVariantList actuatorList=configuration["actuators"].toList();
		const uint32_t asz=actuatorList.size();
		if(mActuators.size()!=asz) {
			mActuators.setSize(asz);
		}
		for(uint32_t i=0; i<asz; ++i) {
			qDebug()<<" "<<i<<" actuator config: ";
			QVariantMap actuatorMap=actuatorList[i].toMap();
			qDebug()<<"actuatorMap: "<<actuatorMap;
			ArduMYActuatorValueRepresentation representation=REPRESENTATION_COUNT;
			{
				ArduMYActuatorConfig &config=mActuators[i].config;
				QVariantMap actuatorConfigMap=actuatorMap["config"].toMap();
				config.type=ardumyActuatorTypeFromString(actuatorConfigMap["type"].toString());
				QString nickStr=actuatorConfigMap["nickName"].toString();
				ardumyActuatorNameFromString(config, nickStr);
				representation=config.representation=ardumyActuatorValueRepresentationFromString(actuatorConfigMap["representation"].toString());
				qDebug()<<"    + config.name: "<< ardumyActuatorNameToString(config);
				qDebug()<<"    + config.type: "<< ardumyActuatorTypeToString(config.type);
				qDebug()<<"    + config.representation: "<< ardumyActuatorValueRepresentationToString(config.representation);
			}
			{
				ArduMYActuatorState &state=mActuators[i].state;
				QVariantMap actuatorStateMap=actuatorMap["state"].toMap();
				state.flags=actuatorStateMap["flags"].toLongLong();
				ardumyActuatorValueFromVariant(state.value, actuatorStateMap["value"], representation);
				qDebug()<<"    + state: "<< ardumyActuatorStateToString(state, representation);
				qDebug()<<"    + value.toFloat(): "<< state.value.toFloat(representation);
			}
		}
	} else {
		qDebug()<<"No actuators found in configuration while loading";
	}
	if(configuration.contains("serial")) {
		mSerialSettings.fromMap(configuration["serial"].toMap());
		qDebug()<<"Loaded serial settings: "<<serialSettingsToString(mSerialSettings);
	} else {
		qDebug()<<"No serial found in configuration while loading";
	}
	// Update UI with any changes
	if(nullptr!=mWidget) {
		mWidget->configure(this);
	}
}

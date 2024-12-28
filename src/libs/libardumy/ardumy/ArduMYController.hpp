#ifndef ARDUMYCONTROLLER_HPP
#define ARDUMYCONTROLLER_HPP

#include "ardumy_arduino/actuator/ArduMYActuatorSet.hpp"
#include "ardumy_arduino/serializer/ArduMYCommandSerializer.hpp"
#include "hardware/controllers/IController.hpp"
#include "hardware/serial/SerialSettings.hpp"

#include <QObject>
#include <QSerialPort>
#include <QVector>
#include <QBitArray>
#include <QTimer>

class SerialSettingsWidget;
class ArduMYControllerWidget;



class ArduMYController : public IController, public QEnableSharedFromThis<ArduMYController>
{
	Q_OBJECT
private:

	SerialSettings mSerialSettings;
	QSerialPort *mSerialInterface;
	ArduMYActuatorSet mActuators;

	ArduMYCommandSerializer mCommandSerializer;

	bool mLimpDirty;
	bool mCountDirty;
	bool mSyncDirty;
	QTimer mSyncTimer;

	ArduMYControllerWidget *mWidget;

public:
	explicit ArduMYController(QObject *parent = nullptr);
	virtual ~ArduMYController() override;

public:

	void setSerialConfig(SerialSettings);
	SerialSettings &serialSettings();
	void openSerialPort();
	void closeSerialPort();

private:
	void syncData();
	void writeData(const QByteArray &data);

	void setHookSignals(bool hook);

private slots:
	void onSendingTimer();

	// Actuator Widget slots
public slots:
	void onActuatorWidgetMoved(quint32 id, ACTUATOR_VALUE val);
	void onActuatorWidgetLimped(quint32 id, bool limp);
	void onActuatorWidgetDeleted(quint32 id);


	// Serial IO slots
private slots:
	void onSerialReadData();
	void onSerialDataWritten(qint64);
	void onSerialHandleError(QSerialPort::SerialPortError error);
	void onSerialSettingsChanged();


	// Arduino spesifics.
	// TODO: Decide how to expose this outside this class. Should it become part of the IServoController interface somehow? What other options exists?
public:
	void configurePin(ACTUATOR_INDEX servoIndex, unsigned char pin);
	ArduMYActuator *addActuator();
	void deleteActuator(quint32 id);
	void setActuatorCount(ACTUATOR_INDEX);
	ArduMYActuatorSet &actuators();
	
	QVariantMap ardumy_configuration();
	void ardumy_setConfiguration(QVariantMap &configuration);
	

	// IController interface
public:
	// Version
	QString firmwareVersion() override;
	// UI
	bool hasConfigurationWidget() const override;
	QWidget *configurationWidget() override;
	// Connection status
	void setConnected(bool) override;
	bool isConnected() override;
	// Actuator definition
	ACTUATOR_INDEX maxActuatorsSupported() override;
	ACTUATOR_INDEX actuatorCount() override;
	QString actuatorName(ACTUATOR_INDEX) override;
	ACTUATOR_VALUE actuatorTargetValue(ACTUATOR_INDEX) override;
	ACTUATOR_VALUE actuatorDefaultValue(ACTUATOR_INDEX) override;
	// Mandatory actuator state
	void setLimp(ACTUATOR_INDEX index, bool limp) override;
	void setTargetValue(ACTUATOR_INDEX index, ACTUATOR_VALUE value) override;
	// Optional actuator state
	void setLimp(const QBitArray &flags) override;
	void setTargetPose(const Pose &pose) override;

	
signals:
	void settingsChanged();
	void connectionChanged();
	void readyToWrite();

};

#endif
// ARDUMYCONTROLLER_HPP

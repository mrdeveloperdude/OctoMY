#ifndef ARDUMYCONTROLLER_HPP
#define ARDUMYCONTROLLER_HPP

#include "hardware/controllers/IController.hpp"

#include "ardumy_arduino/ArduMYCommandSerializer.hpp"
#include "ardumy_arduino/ArduMYActuatorSet.hpp"

#include "hardware/serial/SerialSettings.hpp"

#include <QObject>
#include <QSerialPort>
#include <QVector>
#include <QBitArray>
#include <QTimer>

class SerialSettingsWidget;
class ArduMYControllerWidget;



class ArduMYController : public IController
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
	void onActuatorWidgetMoved(quint32 id, qreal val);
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
	void configurePin(quint8 servoIndex, unsigned char pin);
	ArduMYActuator *addActuator();
	void deleteActuator(quint32 id);
	void setActuatorCount(quint8);
	ArduMYActuatorSet &actuators();
	
	QVariantMap ardumy_configuration();
	void ardumy_setConfiguration(QVariantMap &configuration);
	

	// IController interface
public:
	// Version
	QString version() override;
	// UI
	bool hasConfigurationWidget() const override;
	QWidget *configurationWidget() override;
	// Connection status
	void setConnected(bool) override;
	bool isConnected() override;
	// Actuator definition
	quint8 maxActuatorsSupported() override;
	quint8 actuatorCount() override;
	QString actuatorName(quint8) override;
	qreal actuatorValue(quint8) override;
	qreal actuatorDefault(quint8) override;
	// Mandatory actuator state
	void limp(quint8 index, bool limp) override;
	void move(quint8 index, qreal value) override;
	// Optional actuator state
	void limp(const QBitArray &flags) override;
	void move(const Pose &pose) override;
	void limpAll() override;
	void centerAll() override;
	
	
signals:
	void settingsChanged();
	void connectionChanged();
	void readyToWrite();

};

#endif
// ARDUMYCONTROLLER_HPP

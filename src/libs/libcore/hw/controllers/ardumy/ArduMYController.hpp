#ifndef ARDUMYCONTROLLER_HPP
#define ARDUMYCONTROLLER_HPP

#include "hw/controllers/IActuatorController.hpp"

#include "ardumy/ArduMYCommandSerializer.hpp"
#include "ardumy/ArduMYActuatorSet.hpp"

#include "hw/serial/SerialSettings.hpp"

#include <QObject>
#include <QSerialPort>
#include <QVector>
#include <QBitArray>
#include <QTimer>

class SerialSettingsWidget;
class ArduMYControllerWidget;



class ArduMYController : public IActuatorController
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
	virtual ~ArduMYController();

public:

	void setSerialConfig(SerialSettings);
	SerialSettings &serialSettings();
	void openSerialPort();
	void closeSerialPort();

private:
	void syncData();
	void writeData(const QByteArray &data);

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


	// IActuatorController interface
public:

	void setConnected(bool) Q_DECL_OVERRIDE;
	bool isConnected() Q_DECL_OVERRIDE;


	void limp(QBitArray &flags) Q_DECL_OVERRIDE;
	void limp(quint8 index, bool limp) Q_DECL_OVERRIDE;
	void move(Pose &pose) Q_DECL_OVERRIDE;
	void move(quint8 index, qreal value) Q_DECL_OVERRIDE;
	QString version() Q_DECL_OVERRIDE;
	quint8 maxActuatorsSupported() Q_DECL_OVERRIDE;

	quint8 actuatorCount() Q_DECL_OVERRIDE;
	QString actuatorName(quint8) Q_DECL_OVERRIDE;
	qreal actuatorValue(quint8) Q_DECL_OVERRIDE;
	qreal actuatorDefault(quint8) Q_DECL_OVERRIDE;

	QWidget *configurationWidget() Q_DECL_OVERRIDE;

	QVariantMap configuration() Q_DECL_OVERRIDE;
	void setConfiguration(QVariantMap &configuration) Q_DECL_OVERRIDE;


signals:
	void settingsChanged();
	void connectionChanged();
	void readyToWrite();

};

#endif // ARDUMYCONTROLLER_HPP

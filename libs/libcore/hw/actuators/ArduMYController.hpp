#ifndef ARDUMYCONTROLLER_HPP
#define ARDUMYCONTROLLER_HPP

#include "IActuatorController.hpp"

#include "../arduino/CommandParser.hpp"
#include "SerialSettings.hpp"

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
	QVector<ArduMYActuator> mActuators;
	bool mKillDirty;
	bool mCountDirty;
	bool mSyncDirty;
	QTimer mSyncTimer;

	ArduMYControllerWidget *mWidget;

public:
	explicit ArduMYController(QObject *parent = nullptr);
	virtual ~ArduMYController();

public:
	void setSerialConfig(SerialSettings);
	void openSerialPort();
	void closeSerialPort();

private:
	void syncData();
	void writeData(const QByteArray &data);

	/*
	// Serial IO slots
	private slots:
	void onSerialReadData();
	void onSerialDataWritten(qint64);
	void onSerialHandleError(QSerialPort::SerialPortError error);
	void onSerialSettingsChanged();
	*/

	// Arduino spesifics.
	// TODO: Decide how to expose this outside this class. Should it become part of the IServoController interface somehow? What other options exists?
public:
	void configurePin(quint8 servoIndex, unsigned char pin);

	void setServosCount(quint8);


	// IActuatorController interface
public:

	void setConnected(bool) Q_DECL_OVERRIDE;
	bool isConnected() Q_DECL_OVERRIDE;


	void limp(QBitArray &flags) Q_DECL_OVERRIDE;
	void move(Pose &pose) Q_DECL_OVERRIDE;
	void move(quint8 index, qreal value) Q_DECL_OVERRIDE;
	QString version() Q_DECL_OVERRIDE;
	quint8 maxActuatorsSupported() Q_DECL_OVERRIDE;

	quint8 actuatorCount() Q_DECL_OVERRIDE;
	QString actuatorName(quint8) Q_DECL_OVERRIDE;
	qreal actuatorValue(quint8) Q_DECL_OVERRIDE;
	qreal actuatorDefault(quint8) Q_DECL_OVERRIDE;

	QWidget *configurationWidget() Q_DECL_OVERRIDE;

	QVariantMap confiruation() Q_DECL_OVERRIDE;
	void setConfiguration(QVariantMap &configuration) Q_DECL_OVERRIDE;


signals:
	void settingsChanged();
	void connectionChanged();
	void readyToWrite();

};

#endif // ARDUMYCONTROLLER_HPP

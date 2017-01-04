#ifndef ARDUMYCONTROLLER_HPP
#define ARDUMYCONTROLLER_HPP

#include "IServoController.hpp"


#include "../arduino/Actuator.hpp"

#include <QObject>
#include <QSerialPort>
#include <QVector>
#include <QBitArray>
#include <QTimer>

class SerialSettingsWidget;



class ArduMYController : public IServoController
{
	Q_OBJECT
private:

	SerialSettingsWidget *mSerialSettings;
	QSerialPort *mSerialInterface;
	QVector<Actuator> mActuators;
	bool mKillDirty;
	bool mCountDirty;
	bool mSyncDirty;
	QTimer mSyncTimer;

public:
	explicit ArduMYController(QObject *parent = nullptr);
	virtual ~ArduMYController();

public:
	void configure();
	void openSerialPort();
	void closeSerialPort();

private:
	void syncData();
	void writeData(const QByteArray &data);

	// Serial IO slots
private slots:
	void onSerialReadData();
	void onSerialDataWritten(qint64);
	void onSerialHandleError(QSerialPort::SerialPortError error);
	void onSettingsChanged();

	// Arduino spesifics.
	// TODO: Decide how to expose this outside this class. Should it become part of the IServoController interface somehow? What other options exists?
public:
	void configurePin(quint32 servoIndex, unsigned char pin);

	// IServoController interface
public:

	void setConnected(bool) Q_DECL_OVERRIDE;
	bool isConnected() Q_DECL_OVERRIDE;

	void setServosCount(quint32) Q_DECL_OVERRIDE;
	void kill(QBitArray &flags) Q_DECL_OVERRIDE;
	void move(Pose &pose) Q_DECL_OVERRIDE;
	void fetchVersionData() Q_DECL_OVERRIDE;
	void fetchDebugData() Q_DECL_OVERRIDE;
	quint32 maximumServosSupported() Q_DECL_OVERRIDE;

	signals:
		void settingsChanged();
		void connectionChanged();
		void readyToWrite();

};

#endif // ARDUMYCONTROLLER_HPP

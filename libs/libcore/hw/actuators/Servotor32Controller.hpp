#ifndef SERVOTOR32CONTROLLER_HPP
#define SERVOTOR32CONTROLLER_HPP

#include "IServoController.hpp"

#include <QTimer>

#include <QSerialPort>

class QBitArray;
class SerialSettingsWidget;

// Documentation for serial commands are here: http://arcbotics.com/lessons/servotor32-commands/

class Servotor32Controller : public IServoController
{
public:
	Servotor32Controller();


public:
	const static quint32 SERVO_COUNT=32;
private:

	SerialSettingsWidget *settings;
	QSerialPort *serial;
	QByteArray inBuf;
	quint32 lastPos[SERVO_COUNT];
	quint32 dirtyMoveFlags;
	QTimer sendTimer;

public:
	explicit Servotor32Controller(QObject *parent = nullptr);
	virtual ~Servotor32Controller();

public:
	void configure();
	void openSerialPort();
	void closeSerialPort();
	bool isConnected();

private slots:
	void syncMove();
	void writeData(const QByteArray &data);



	// Serial IO slots
private slots:
	void onSerialReadData();
	void onSerialDataWritten(qint64);
	void onSerialHandleError(QSerialPort::SerialPortError error);
	void onSettingsChanged();


	// IServoController interface
public:

	void kill(QBitArray &flags) Q_DECL_OVERRIDE;
	void centerAll() Q_DECL_OVERRIDE;
	void version() Q_DECL_OVERRIDE;
	void debug() Q_DECL_OVERRIDE;
	void move(Pose &pose) Q_DECL_OVERRIDE;

signals:
	void settingsChanged();
	void connectionChanged();
	void readyToWrite();

};







#endif // SERVOTOR32CONTROLLER_HPP

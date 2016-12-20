#ifndef SERVOTOR32CONTROLLER_HPP
#define SERVOTOR32CONTROLLER_HPP

#include "IServoController.hpp"

#include <QTimer>

#include <QSerialPort>
#include <QBitArray>
#include <QVector>

class SerialSettingsWidget;

// Documentation for serial commands are here: http://arcbotics.com/lessons/servotor32-commands/

class Servotor32Controller: public IServoController
{
	Q_OBJECT
private:
	SerialSettingsWidget *mSerialSettings;
	QSerialPort *mSerialInterface;
	QVector<qreal> mAccumulatedPosition;
	QBitArray mDirtyMoveFlags;

public:
	explicit Servotor32Controller(QObject *parent = nullptr);
	virtual ~Servotor32Controller();

public:
	void configure();
	void openSerialPort();
	void closeSerialPort();

private:
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

	void setConnected(bool) Q_DECL_OVERRIDE;
	bool isConnected() Q_DECL_OVERRIDE;

	void kill(QBitArray &flags) Q_DECL_OVERRIDE;
	void move(Pose &pose) Q_DECL_OVERRIDE;
	void killAll() Q_DECL_OVERRIDE;
	void centerAll() Q_DECL_OVERRIDE;
	void fetchVersionData() Q_DECL_OVERRIDE;
	void fetchDebugData() Q_DECL_OVERRIDE;


signals:
	void settingsChanged();
	void connectionChanged();
	void readyToWrite();

};







#endif // SERVOTOR32CONTROLLER_HPP

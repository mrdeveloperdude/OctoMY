#ifndef SERVOTOR32CONTROLLER_HPP
#define SERVOTOR32CONTROLLER_HPP

#include "IActuatorController.hpp"

#include <QTimer>

#include <QSerialPort>
#include <QBitArray>
#include <QVector>

class SerialSettingsWidget;

// Documentation for serial commands are here: http://arcbotics.com/lessons/servotor32-commands/

class Servotor32Controller: public IActuatorController
{
	Q_OBJECT
private:
	SerialSettingsWidget *mSerialSettings;
	QSerialPort *mSerialInterface;
	QVector<qreal> mAccumulatedPosition;
	QBitArray mDirtyMoveFlags;
	QByteArray mInputBuffer;
	quint32 mReads;

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

private:

	void debug();

	// Serial IO slots
private slots:
	void onSerialReadData();
	void onSerialDataWritten(qint64);
	void onSerialHandleError(QSerialPort::SerialPortError error);
	void onSettingsChanged();


	// IActuatorController interface
public:

	void setConnected(bool) Q_DECL_OVERRIDE;
	bool isConnected() Q_DECL_OVERRIDE;

	void limp(QBitArray &flags) Q_DECL_OVERRIDE;
	void move(Pose &pose) Q_DECL_OVERRIDE;
	void move(quint8 index, qreal value) Q_DECL_OVERRIDE;
	void limpAll() Q_DECL_OVERRIDE;
	void centerAll() Q_DECL_OVERRIDE;
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


#endif // SERVOTOR32CONTROLLER_HPP

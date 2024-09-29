#ifndef SERVOTOR32CONTROLLER_HPP
#define SERVOTOR32CONTROLLER_HPP

#include "hardware/controllers/IActuatorController.hpp"
#include "hardware/serial/SerialSettings.hpp"

#include <QTimer>


#include <QBitArray>
#include <QVector>


struct Servotor32ActuatorStanza {
	int klol;
};

class SerialSettings;
class Servotor32ControllerWidget;

// Documentation for serial commands are here: http://arcbotics.com/lessons/servotor32-commands/

class Servotor32Controller: public IActuatorController
{
	Q_OBJECT
private:
	QVector<qreal> mAccumulatedPosition;
	QBitArray mDirtyMoveFlags;
	QByteArray mInputBuffer;
	quint32 mReads;

	Servotor32ControllerWidget *mWidget;
	SerialSettings mSerialSettings;
	QSerialPort *mSerialInterface;
	QList<Servotor32ActuatorStanza> mActuatorStanzas;

public:
	explicit Servotor32Controller(QObject *parent = nullptr);
	virtual ~Servotor32Controller() override;

public:
	void configure(SerialSettings &serialSettings);
	void openSerialPort();
	void closeSerialPort();

private:
	void syncMove();
	void writeData(const QByteArray &data);


	void setHookSignals(bool hook);
private:

	void debug();

	// Serial IO slots
private slots:
	void onSerialReadData();
	void onSerialDataWritten(qint64);
	void onSerialHandleError(QSerialPort::SerialPortError error);
	void onSerialSettingsChanged();


	// IActuatorController interface
public:

	void setConnected(bool) override;
	bool isConnected() override;

	void limp(QBitArray &flags) override;
	void limp(quint8 index, bool limp) override;
	void move(Pose &pose) override;
	void move(quint8 index, qreal value) override;
	void limpAll() override;
	void centerAll() override;
	QString version() override;
	quint8 maxActuatorsSupported() override;

	quint8 actuatorCount() override;
	QString actuatorName(quint8) override;
	qreal actuatorValue(quint8) override;
	qreal actuatorDefault(quint8) override;

	QWidget *configurationWidget() override;

	QVariantMap configuration() override;
	void setConfiguration(QVariantMap &configuration) override;


signals:
	void settingsChanged();
	void connectionChanged();
	void readyToWrite();

};


#endif
// SERVOTOR32CONTROLLER_HPP

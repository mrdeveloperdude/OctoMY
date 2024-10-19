#ifndef SERVOTOR32CONTROLLER_HPP
#define SERVOTOR32CONTROLLER_HPP

#include "hardware/controllers/IController.hpp"
#include "hardware/serial/SerialSettings.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QTimer>


#include <QBitArray>
#include <QVector>


struct Servotor32ActuatorStanza {
	int klol;
};

struct SerialSettings;
class Servotor32ControllerWidget;

// Documentation for serial commands are here: http://arcbotics.com/lessons/servotor32-commands/

class Servotor32Controller: public IController
{
	Q_OBJECT
private:
	friend class Servotor32ControllerWidget;
private:
	static const int SERVO_COUNT;
	QVector<qreal> mAccumulatedPosition;
	QBitArray mDirtyMoveFlags;
	QByteArray mInputBuffer;
	quint32 mReads;
	Servotor32ControllerWidget *mWidget{nullptr};
	SerialSettings mSerialSettings;
	QSerialPort *mSerialInterface{nullptr};
	QList<Servotor32ActuatorStanza> mActuatorStanzas;
	QString mLastSerialSideVersion;
	QString mLastSerialError;
	bool mAskedVersion{false};
	ConfigureHelper mConfigureHelper;

public:
	explicit Servotor32Controller(QObject *parent = nullptr);
	virtual ~Servotor32Controller() override;
	
	// Servotor32 related
private:
	SerialSettings s32_defaultSerialSettings();
	void s32_configureSerial(SerialSettings &serialSettings);
	void s32_openSerialPort();
	void s32_closeSerialPort();
	void s32_setHookSerialSignals(bool hook);
	void s32_syncMove();
	void s32_writeData(const QByteArray &data);
	void s32_version();
	void s32_debug();
	
	// Serial IO slots
private slots:
	void onSerialReadData();
	void onSerialDataWritten(qint64);
	void onSerialHandleError(QSerialPort::SerialPortError error);
	void onSerialSettingsChanged();
	
	// IController interface
public:
	// Version
	QString version() override;
	// UI
	bool hasConfigurationWidget() const override;
	QWidget *configurationWidget() override;
	// Configuration management
	QVariantMap configuration() override;
	void setConfiguration(const QVariantMap &map) override;
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
	QString debugString() override;

signals:
	void settingsChanged();
	void connectionChanged();
	void readyToWrite();

};


#endif
// SERVOTOR32CONTROLLER_HPP

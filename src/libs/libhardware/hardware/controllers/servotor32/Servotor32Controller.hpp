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
	static const ACTUATOR_INDEX SERVO_COUNT;
	QVector<ACTUATOR_VALUE> mAccumulatedPosition;
	QBitArray mDirtyMoveFlags;
	QBitArray mLimp;
	QByteArray mInputBuffer;
	quint32 mReads;
	Servotor32ControllerWidget *mWidget{nullptr};
	SerialSettings mSerialSettings;
	QSerialPort *mSerialInterface{nullptr};
	QList<Servotor32ActuatorStanza> mActuatorStanzas;
	QString mLastSerialSideVersion;
	QString mLastSerialError;
	bool mAskedVersion{false};
	bool mBinaryMode{false};
	bool mDebug{false};
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
	void s32_syncMoveText();
	void s32_syncMoveBinary();
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
	QString firmwareVersion() override;
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
	ACTUATOR_INDEX maxActuatorsSupported() override;
	ACTUATOR_INDEX actuatorCount() override;
	QString actuatorName(ACTUATOR_INDEX) override;
	ACTUATOR_VALUE actuatorTargetValue(ACTUATOR_INDEX) override;
	ACTUATOR_VALUE actuatorDefaultValue(ACTUATOR_INDEX) override;
	// Mandatory actuator state
	bool isLimp(ACTUATOR_INDEX index) override;
	void setLimp(ACTUATOR_INDEX index, bool limp) override;
	void setTargetValue(ACTUATOR_INDEX index, ACTUATOR_VALUE value) override;
	// Optional actuator state
	void setLimp(const QBitArray &flags) override;
	void setTargetPose(const Pose &pose) override;
	void toggleLimpAll(bool limp) override;
	void centerAll() override;
	QString debugString() override;

signals:
	void settingsChanged();
	void connectionChanged();
	void readyToWrite();
	void errorOccurred(const QString &error);
};


#endif
// SERVOTOR32CONTROLLER_HPP

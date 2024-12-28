#ifndef ICONTROLLER_H
#define ICONTROLLER_H

#include "pose/Pose.hpp"

#include <QObject>
#include <QVariantMap>
#include <QBitArray>

class QBitArray;
class QWidget;


typedef quint16 ACTUATOR_INDEX;
typedef double ACTUATOR_VALUE;

// TODO: Verify that the cast works
#define ACTUATOR_FLAG_SELECTOR(NAME_GET, NAME_SET, BIT)  \
	inline bool NAME_GET() const { return ( flags & (1<<(BIT)) ) > 0; } \
	inline void NAME_SET(bool val) { if(val) { flags=flags|(1<<(BIT)); } else { flags=flags&static_cast<quint32>(~(1<<(BIT))); } }



struct ActuatorConfiguration {
	QString name;
	ACTUATOR_VALUE low{0.0f};
	ACTUATOR_VALUE high{1.0f};
	quint32 flags{0};

	// Linear as opposed to rotary means that the actuator acts in a straigt motion similar to a hydraulic cylinder instead of a rotary motion similar to an RC servo
	ACTUATOR_FLAG_SELECTOR(isLinear,			setLinear,		0 )

	// Continuous as opposed to ranged means that the actuator continues running without any stop point, as a motor instead of a servo
	ACTUATOR_FLAG_SELECTOR(isContinuous,		setContinuous,	1 )

	// Reverse means that the actuator direction has been reversed
	ACTUATOR_FLAG_SELECTOR(isReverse,			setReverse,		2 )

};


enum ActuatorMotion{
	CONTINUOUS_MOTION, RANGED_MOTION
};


/*!
 * \brief The IServoController class is an interface for Servo controller hardware such as Servotor32
 */

class IController: public QObject
{
	Q_OBJECT

private:
	const QString mTitle;
	const QString mDescription;
	const QString mIcon;
	QVariantMap mSettingsImpl;
	QBitArray mEnabled;
	//QVector<ActuatorConfiguration> mActuatorConfig;

protected:
	bool mDebug{false};
	QVariantMap mSettings;

public:
	explicit IController(const QString &title, const QString &description, const QString &icon, QObject *parent=nullptr);
	virtual ~IController();

	// IController wrapping
public:
	QString controllerTitle() const;
	QString controllerDescription() const;
	QString controllerIcon() const;
	// The version of this driver. \sa firmwareVersion()
	QString controllerVersion();

	// Debug mode is not virtual
	bool isDebug();
	void setDebug(bool);

	// IController interface
public:
	// The version reported by the controller firmware. \sa controllerVersion()
	virtual QString firmwareVersion();
	
	// Configuration widget is opt-in. Default to no widget
	virtual bool hasConfigurationWidget() const;
	virtual QWidget *configurationWidget();
	
	// Configuration processing is opt-in. Default to no internal configuration processing
	virtual QVariantMap configuration();
	virtual void setConfiguration(const QVariantMap &map);
	
	// Enabled state
	virtual void setEnabled(ACTUATOR_INDEX index, bool enabled);
	virtual bool isEnabled(ACTUATOR_INDEX index) const;
	
	// Management of connection is mandatory
	virtual void setConnected(bool) = 0;
	virtual bool isConnected() = 0;
	
	// Management of actuator definitions is opt-in. Default to no actuator support.
public:
	virtual ACTUATOR_INDEX maxActuatorsSupported();
	virtual ACTUATOR_INDEX actuatorCount();
	virtual QString actuatorName(ACTUATOR_INDEX index);
	virtual ACTUATOR_VALUE actuatorTargetValue(ACTUATOR_INDEX index);
	virtual ACTUATOR_VALUE actuatorDefaultValue(ACTUATOR_INDEX index);
	virtual ActuatorMotion actuatorMotion(ACTUATOR_INDEX index);
	
	// Management of basic actuator state is opt-in
public slots:
	virtual bool isLimp(ACTUATOR_INDEX index);
	virtual void setLimp(ACTUATOR_INDEX index, bool limp);
	virtual void setTargetValue(ACTUATOR_INDEX index, ACTUATOR_VALUE value);
	
	// Optional optimizations that default to a general aproach. Implementation encouraged if it will save resources
	virtual void setLimp(const QBitArray &flags);
	virtual void setTargetPose(const Pose &pose);
	virtual void center(ACTUATOR_INDEX index);
	virtual void toggleLimpAll(bool limp);
	virtual void centerAll();
	virtual QString debugString();
	
	// Management of sensor definitions is opt-in. Default to no sensor support.
public:
	virtual ACTUATOR_INDEX maxSensorsSupported();
	virtual ACTUATOR_INDEX sensorCount();
	virtual QString sensorName(ACTUATOR_INDEX index);
	// Management of sensor state is opt-in. Default to no sensor support.
	virtual ACTUATOR_VALUE sensorValue(ACTUATOR_INDEX index);
	
	// Management of lobe definitions is opt-in. Default to no lobe support.
public:
	virtual bool lobesSupported();
	virtual bool setLobe(ACTUATOR_INDEX index, const QByteArray &lobeData);
	virtual QString getLobeTitle(ACTUATOR_INDEX index);
	virtual QString getLobeVersion(ACTUATOR_INDEX index);
	virtual QString getLobeID(ACTUATOR_INDEX index);
	
	// Management of lobe state is opt-in. Default to no lobe support.
	virtual void getLobeEnabled(ACTUATOR_INDEX index, bool enabled);

signals:
	void definitionsChanged();

};

#endif
// ICONTROLLER_HPP

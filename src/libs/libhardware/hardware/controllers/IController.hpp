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
	QString controllerVersion();

	// Debug mode is not virtual
	bool isDebug();
	void setDebug(bool);

	// IController interface
public:
	// Management of version is mandatory
	virtual QString version() = 0;
	
	// Default to no widget
	virtual bool hasConfigurationWidget() const;
	virtual QWidget *configurationWidget();
	
	// Default to no internal configuration processing
	virtual QVariantMap configuration();
	virtual void setConfiguration(const QVariantMap &map);
	
	// Enabled state
	virtual void setEnabled(ACTUATOR_INDEX index, bool enabled);
	virtual bool isEnabled(ACTUATOR_INDEX index) const;
	
	// Management of connection is mandatory
	virtual void setConnected(bool) = 0;
	virtual bool isConnected() = 0;
	
	// Management of actuator definitions is mandatory
	virtual ACTUATOR_INDEX maxActuatorsSupported() = 0;
	virtual ACTUATOR_INDEX actuatorCount() = 0;
	virtual QString actuatorName(ACTUATOR_INDEX) = 0;
	virtual ACTUATOR_VALUE actuatorTargetValue(ACTUATOR_INDEX) = 0;
	virtual ACTUATOR_VALUE actuatorDefaultValue(ACTUATOR_INDEX) = 0;
	virtual ActuatorMotion actuatorMotion(ACTUATOR_INDEX);
	
	// Management of basic actuator state is mandatory
public slots:
	virtual void setLimp(ACTUATOR_INDEX index, bool limp) = 0;
	virtual bool isLimp(ACTUATOR_INDEX index) = 0;
	virtual void setTargetValue(ACTUATOR_INDEX index, ACTUATOR_VALUE value) = 0;
	
	// Optional optimizations that default to a general aproach. Implementation encouraged if it will save resources
	virtual void setLimp(const QBitArray &flags);
	virtual void setTargetPose(const Pose &pose);
	virtual void center(ACTUATOR_INDEX index);
	virtual void toggleLimpAll(bool limp);
	virtual void centerAll();
	virtual QString debugString();

signals:
	void configurationChanged();

};

#endif
// ICONTROLLER_HPP

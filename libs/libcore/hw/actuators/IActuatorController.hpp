#ifndef IACTUATORCONTROLLER_HPP
#define IACTUATORCONTROLLER_HPP

#include "puppet/Pose.hpp"
#include "widgets/TryToggleState.hpp"


#include <QObject>
#include <QVariantMap>


class QBitArray;
class QWidget;


#define IACTUATOR_FLAG_SELECTOR(NAME_GET, NAME_SET, BIT)  \
	inline bool NAME_GET() const { return ( flags & (1<<(BIT)) ) > 0; } \
	inline void NAME_SET(bool val) { if(val) { flags=flags|(1<<(BIT)); } else { flags=flags&(~(1<<(BIT))); } }



struct IActuatorConfiguration {
	QString name;
	qreal low;
	qreal high;
	quint32 flags;

	IACTUATOR_FLAG_SELECTOR(isLinear,			setLinear,		0 ) // Linear as opposed to rotary means that the actuator acts in a straigt motion similar to a hydraulic cylinder instead of a rotary motion similar to an RC servo
	IACTUATOR_FLAG_SELECTOR(isContinuous,		setContinuous,	1 ) // Continuous as opposed to ranged means that the actuator continues running without any stop point, as a motor instead of a servo
	IACTUATOR_FLAG_SELECTOR(isReverse,			setReverse,		2 ) // Reverse means that the actuator direction has been reversed

};

struct ActuatorControllerConfiguration:QVector<IActuatorConfiguration> {

};


/*!
 * \brief The IServoController class is an interface for Servo controller hardware such as Servotor32
 */

class IActuatorController: public QObject
{
	Q_OBJECT

private:
	QString mName;

public:
	explicit IActuatorController(QString name, QObject *parent=nullptr);
	virtual ~IActuatorController();

public:


	QString controllerName();
	QString controllerVersion();
	bool isDebug();
	void setDebug(bool);

	// IActuatorController interface

public:

	virtual void setConnected(bool)=0;
	virtual bool isConnected()=0;
	virtual void limp(QBitArray &flags)=0;
	virtual void move(Pose &pose)=0;
	virtual void move(quint8 index, qreal value)=0;
	// NOTE: Why do we bother to spell out all these? Because the underlying hardware may optimize the communication for them (or not).
	virtual void limpAll();
	virtual void centerAll();
	virtual QString version()=0;
	virtual quint8 maxActuatorsSupported()=0;

	virtual quint8 actuatorCount()=0;
	virtual QString actuatorName(quint8)=0;
	virtual qreal actuatorValue(quint8)=0;
	virtual qreal actuatorDefault(quint8)=0;

	virtual QWidget *configurationWidget();

	virtual QVariantMap confiruation();
	virtual void setConfiguration(QVariantMap &configuration);


signals:

	void actuatorConfigurationChanged();

	/*
	 *
	Planned changes:

	 + Rename term "kill" to "limp"
	 + Rename term "servo" to "actuator"
	 + Add "get UI widget"
	 + Remove "setServosCount()"
	 + Add getConfiguration
	 + Add configurationChanged() signal


	*/

};

#endif // IACTUATORCONTROLLER_HPP

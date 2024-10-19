#ifndef ICONTROLLER_H
#define ICONTROLLER_H

#include "pose/Pose.hpp"

#include <QObject>
#include <QVariantMap>


class QBitArray;
class QWidget;

// TODO: Verify that the cast works
#define IACTUATOR_FLAG_SELECTOR(NAME_GET, NAME_SET, BIT)  \
	inline bool NAME_GET() const { return ( flags & (1<<(BIT)) ) > 0; } \
	inline void NAME_SET(bool val) { if(val) { flags=flags|(1<<(BIT)); } else { flags=flags&static_cast<quint32>(~(1<<(BIT))); } }



struct IActuatorConfiguration {
	QString name;
	qreal low;
	qreal high;
	quint32 flags;

	// Linear as opposed to rotary means that the actuator acts in a straigt motion similar to a hydraulic cylinder instead of a rotary motion similar to an RC servo
	IACTUATOR_FLAG_SELECTOR(isLinear,			setLinear,		0 )

	// Continuous as opposed to ranged means that the actuator continues running without any stop point, as a motor instead of a servo
	IACTUATOR_FLAG_SELECTOR(isContinuous,		setContinuous,	1 )

	// Reverse means that the actuator direction has been reversed
	IACTUATOR_FLAG_SELECTOR(isReverse,			setReverse,		2 )

};

struct ActuatorControllerConfiguration:QVector<IActuatorConfiguration> {

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

protected:
	bool mDebug{false};
	QVariantMap mSettings;

public:
	explicit IController(QString title, QString description, QString icon, QObject *parent=nullptr);
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
	
	// Management of connection is mandatory
	virtual void setConnected(bool) = 0;
	virtual bool isConnected() = 0;
	
	// Management of actuator definitions is mandatory
	virtual quint8 maxActuatorsSupported() = 0;
	virtual quint8 actuatorCount() = 0;
	virtual QString actuatorName(quint8) = 0;
	virtual qreal actuatorValue(quint8) = 0;
	virtual qreal actuatorDefault(quint8) = 0;
	
	// Management of basic actuator state is mandatory
	virtual void limp(quint8 index, bool limp) = 0;
	virtual void move(quint8 index, qreal value) = 0;
	
	// Optional optimizations that default to a general aproach. Implementation encouraged if it will save resources
	virtual void limp(const QBitArray &flags);
	virtual void move(const Pose &pose);
	virtual void center(quint8 index);
	virtual void limpAll();
	virtual void centerAll();
	virtual QString debugString();

signals:
	void configurationChanged();

};

#endif
// ICONTROLLER_HPP

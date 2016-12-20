#ifndef ISERVOCONTROLLER_HPP
#define ISERVOCONTROLLER_HPP

#include "puppet/Pose.hpp"
#include "widgets/TryToggleState.hpp"


#include <QObject>

class QBitArray;

/*!
 * \brief The IServoController class is an interface for Servo controller hardware such as Servotor32
 */

class IServoController: public QObject
{
	Q_OBJECT

private:
	QString mName;

public:
	explicit IServoController(QString name, QObject *parent=nullptr);
	virtual ~IServoController();

public:


	QString controllerName();
	QString controllerVersion();
	bool isDebug();
	void setDebug(bool);

	// IServoController interface

public:


	virtual void setConnected(bool)=0;
	virtual bool isConnected()=0;
	virtual void kill(QBitArray &flags)=0;
	virtual void move(Pose &pose)=0;
	// NOTE: Why do we bother to spell out all these? Because the underlying hardware may optimize the communication for them (or not).
	virtual void killAll();
	virtual void centerAll();
	virtual void fetchVersionData()=0;
	virtual void fetchDebugData()=0;


};

#endif // ISERVOCONTROLLER_HPP

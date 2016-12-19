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
	IServoController(QString name, QObject *parent=nullptr);

public:


	QString controllerName();
	QString controllerVersion();
	bool isDebug();
	void setDebug(bool);



	// IServoController interface
public:

	virtual void killAll();

	virtual void kill(QBitArray &flags);
	virtual void centerAll();
	virtual void version();
	virtual void debug();
	virtual void move(Pose &pose);

};

#endif // ISERVOCONTROLLER_HPP

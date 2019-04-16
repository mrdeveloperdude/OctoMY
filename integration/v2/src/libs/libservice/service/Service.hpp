#ifndef SERVICE_HPP
#define SERVICE_HPP

#include <QString>
#include <QStringList>

#include <functional>


typedef std::function<void(bool)> ServiceActivatedCallback;

/**
 * @brief The Service class represents one running part of the program that may be switched on or off. As long as it is on, it will do it's work quietly in the background
 * and providing support for the parts of the application that needs it.
 *
 * Services may also express dependencies on other services. It is the task of the ServiceManager to make sure a service is never started unless the required
 * dependencies have been started first.
 *
 * Services are are decoupled by identifying them by a unique string "name".
 */
class Service
{
private:
	QString mName;
	QStringList mDependencies;

public:
	explicit Service(QString name, QStringList dependencies=QStringList());
	virtual ~Service();

public:
	QString name();
	QStringList dependencies();

	// Service interface.
public:
	virtual void serviceActivate(bool on, ServiceActivatedCallback callBack) =0;
	virtual bool serviceActivated() =0;

};

#endif
// SERVICE_HPP

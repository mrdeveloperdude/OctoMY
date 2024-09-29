#ifndef SERVICE_HPP
#define SERVICE_HPP

#include <QObject>
#include <QString>
#include <QStringList>

#include <functional>


typedef std::function<void(bool on, bool ok)> ServiceActivatedCallback;

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
	// TODO: Should dependencies be determined when registering service with manager rather than in service constructor?
	QStringList mDependencies;
	bool mActiveWanted;
	bool mActive;
	int mRegistrationOrder{-1};

public:
	explicit Service(QString name, QStringList dependencies=QStringList());
	virtual ~Service();

public:
	QString name() const;
	QStringList dependencies() const;

	// Service interface external.
public:
	void serviceChangeActivation(bool on, ServiceActivatedCallback callBack=nullptr);
	bool serviceActiveWanted() const;
	bool serviceActiveActual() const;
	void setRegistrationOrder(int order);
	int registrationOrder();

	// Service interface internal.
protected:
	virtual void serviceActivateImp(bool on, ServiceActivatedCallback callBack=nullptr) =0;
//	virtual bool serviceActivatedImp() const =0 ;

};

#endif
// SERVICE_HPP

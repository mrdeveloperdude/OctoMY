#ifndef SERVICELEVELMANAGER_HPP
#define SERVICELEVELMANAGER_HPP

#include "uptime/ConfigureHelper.hpp"

#include <QObject>
#include <QMap>
#include <QList>
#include <QString>
#include <QSharedPointer>

#include <functional>

class ServiceLevel;
class ServiceManager;
class Service;

class ServiceLevelManager: public QObject
{
	Q_OBJECT
private:
	QMap <QString, QSharedPointer<ServiceLevel> > mServiceLevelsMap;
	QList <QSharedPointer<ServiceLevel> > mServiceLevelsList;
	QSharedPointer<ServiceManager> mServiceManager;
	ConfigureHelper mConfigureHelper;
	int mRegistrationOrderCounter{0};

public:
	explicit ServiceLevelManager();
	virtual ~ServiceLevelManager();

	// ConfigureHelper interface
public:
	void configure();

	// Helpers
public:
	// Return the service for a name, if any
	QSharedPointer <Service> serviceByName(const QString name) const;
	// Return the service level for a name, if any
	QSharedPointer <ServiceLevel> serviceLevelByName(const QString name) const;
	// Return the service manager
	// NOTE: Please beware that changes you make directly on this internal serive manager may interfere destructively with service level manager operation
	QSharedPointer<ServiceManager> serviceManager();
	// Is there a service registered for the given name?
	bool isServiceRegistered(const QString name) const;
	// Is the service provided already registered?
	bool isServiceRegistered(const QSharedPointer <Service> serviceByName) const;
	// Is there a service level registered for the given name?
	bool isServiceLevelRegistered(const QString name) const;
	// Is the service level provided already registered?
	bool isServiceLevelRegistered(const QSharedPointer <ServiceLevel> serviceLevelByName) const;
	// Return baked set of service we want enabled
	QSet<QString> wantedServices() const;


	// Registering and unregistering of services
public:
	// Register the given service with the manager
	bool registerService(const QSharedPointer <Service> service);
	// Un-register the given service from the manager
	bool unRegisterService(const QSharedPointer <Service> service);
	// Un-register the named service from the manager
	bool unRegisterService(const QString name);


	// Registering and unregistering of service levels
public:
	// Register the given service level with the manager
	bool registerServiceLevel(QSharedPointer<ServiceLevel> serviceLevel);
	// Un-register the given service level from the manager
	bool unRegisterServiceLevel(QSharedPointer<ServiceLevel> serviceLevel);
	// Un-register the named service level from the manager
	bool unRegisterServiceLevel(const QString name);

	// Control service manager from service level manager
public:
	// Apply our current idea od the world on the underlying service manager as politely as possible
	void synchronizeServiceManager(const std::function<void(const bool ok)> callBack=nullptr);
	// Enable or disable the named service level depending on on parameter (the level is expected to be registered)
	void enableLevel(const QString name, const bool on, const std::function<void(const bool ok)> callBack=nullptr);
	// Enable or disable the named service levels depending on on parameter (the level is expected to be registered)
	void enableLevels(const QSet<QString> names, const bool on, const std::function<void(const bool ok)> callBack=nullptr);

public slots:
	void onServiceLevelChanged();

signals:
	void servicesChanged();


};

#endif
// SERVICELEVELMANAGER_HPP

#ifndef SERVICEMANAGER_HPP
#define SERVICEMANAGER_HPP

#include "Service.hpp"

#include <QSharedPointer>

#include <QMap>
#include <QList>
#include <QString>
#include <QSharedPointer>

class Service;

class ServiceManager
{
private:
	QMap <QString, QSharedPointer<Service> > mServicesMap;
	QList <QSharedPointer<Service> > mServicesList;

	// Internal helpers
private:
	QSet<QString> nextActivatableInSet(const QSet<QString> set, const bool active);

	// Helpers
public:
	// Is the given name well formed?
	bool nameIsWellformed(const QString name);
	// Is there a service registered for the given name?
	bool registered(const QString name);
	// Is the service provided already registered?
	bool registered(const QSharedPointer <Service> serviceByName);
	// Is the service named activated?
	bool activated(const QString name);
	// Return the number of registered services
	int count();
	// Return the service for a name, if any
	QSharedPointer <Service> serviceByName(const QString name);
	// Return set of all dependencies for a named service
	QSet<QString> dependencies(const QString name);
	// Return set of all dependencies for a provided service
	QSet<QString> dependencies(const QSharedPointer <Service> service);
	// Return the set of all dependents for a named service
	QSet<QString> dependents(QString name);
	// Return the set of all dependents for a provided service
	QSet<QString> dependents(const QSharedPointer <Service> service);
	// Return set of dependencies that are either met or unmet for given service depending on the parameter
	QSet<QString> metDependencies(const QString name, const bool met);
	// Return set of dependents that are either met or unmet for given service depending on the parameter
	QSet<QString> metDependents(const QString name, const bool met);
	// Return the set containing all services
	QSet<QString> all();
	// Show internal state
	void dump();

	// Registering and unregistering of services
public:
	// Register the given service with the manager
	bool registerService(const QSharedPointer <Service> service);
	// Un-register the given service from the manager
	bool unRegisterService(const QSharedPointer <Service> service);
	// Un-register the named service from the manager
	bool unRegisterService(const QString name);


	// Dependency management and service activation
public:
	// Does the provided service exists and does it have it's stated dependencies met?
	bool dependenciesMet(const QSharedPointer <Service> service);
	// Does the named service exists and does it have it's stated dependencies met?
	bool dependenciesMet(const QString name);

	// Does the provided service exist and does any other service depend on it?
	bool dependentsMet(const QSharedPointer <Service> service);
	// Does the named service exists and does any other service depend on it?
	bool dependentsMet(const QString name);

	// (de)activate the services in the provided set, calling provided callback upon completion
	// NOTE: This will recursively (de)activate dependencies and resolve the correct activation order.
	void activate(const QSet<QString> set, const bool active, ServiceActivatedCallback callBack);

};

#endif
// SERVICEMANAGER_HPP

#ifndef SERVICEMANAGER_HPP
#define SERVICEMANAGER_HPP

#include "Service.hpp"

#include <QMap>
#include <QList>
#include <QString>
#include "uptime/SharedPointerWrapper.hpp"

class Service;

typedef std::function<void(bool ok)> ServiceManagerCallback;

class ServiceManager: public QObject
{
	Q_OBJECT
private:
	QMap <QString, QSharedPointer<Service> > mServicesMap;
	QList <QSharedPointer<Service> > mServicesList;

public:
	explicit ServiceManager();
	virtual~ServiceManager();


	// Internal helpers
private:
	QSet<QString> nextActivatableInSet(const QSet<QString> set, const bool active) const;

	// Helpers
public:
	// Is the given name well formed?
	bool nameIsWellformed(const QString name) const;
	// Is there a service registered for the given name?
	bool registered(const QString name) const;
	// Is the service provided already registered?
	bool registered(const QSharedPointer <Service> serviceByName) const;
	// Do we want the the named service to be active?
	bool activatedWanted(const QString name) const;
	// Is the the named service actually active?
	bool activatedActual(const QString name) const;
	// Return the number of registered services
	int count() const;
	// Return if all the named services in the set are active/inactive depending on the parameter
	bool met(const QSet<QString> set, const bool met, const bool actual=false) const;
	// Return all the named services in the set that are active/inactive depending on the parameter
	QSet<QString> metSet(const QSet<QString> set, const bool met, const bool actual=false) const;

	// Return the service for a name, if any
	QSharedPointer <Service> serviceByName(const QString name) const;
	// Return set of all dependencies for a named service
	QSet<QString> dependencies(const QString name, const bool recurse=true) const;
	// Return set of all dependencies for a provided service
	QSet<QString> dependencies(const QSharedPointer <Service> service) const;
	// Return the set of all dependents for a named service
	QSet<QString> dependents(QString name) const;
	// Return the set of all dependents for a provided service
	QSet<QString> dependents(const QSharedPointer <Service> service) const;
	// Return the set containing all services
	QSet<QString> all() const;
	// Return the set containing all services that are either active or inactive depending on the given state
	QSet<QString> all(const bool on, const bool actual=false) const;
	// Show internal state
	void dump() const;
	// Expand the given set to include all dependencies recursively
	QSet<QString> expand(const QSet<QString> set) const;


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
	bool dependenciesMet(const QSharedPointer <Service> service) const;
	// Does the named service exists and does it have it's stated dependencies met?
	bool dependenciesMet(const QString name) const;

	// Does the provided service exist and does any other service depend on it?
	bool dependentsMet(const QSharedPointer <Service> service) const;
	// Does the named service exists and does any other service depend on it?
	bool dependentsMet(const QString name) const;

	// Activate and deactivate services in the provided sets calling provided callback upon completion
	// NOTE: This will recursively (de)activate dependencies and resolve the correct activation order.
	void changeActivation(const QSet<QString> activateSet, const QSet<QString> deactivateSet, ServiceManagerCallback callBack=nullptr);
	// (de)activate the services in the provided set, calling provided callback upon completion
	// NOTE: This will recursively (de)activate dependencies and resolve the correct activation order.
	void changeActivation(const QSet<QString> set, const bool active, ServiceManagerCallback callBack=nullptr);
	// Convenience overload
	void changeActivation(const QString service, const bool active, ServiceManagerCallback callBack=nullptr);

signals:
	void servicesChanged();


};

#endif
// SERVICEMANAGER_HPP

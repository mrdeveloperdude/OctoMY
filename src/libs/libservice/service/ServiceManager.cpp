#include "ServiceManager.hpp"

#include "Service.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QDebug>
#include <QSharedPointer>

ServiceManager::ServiceManager()
	: QObject (nullptr)
{
	OC_METHODGATE();
}


ServiceManager::~ServiceManager()
{
	OC_METHODGATE();
}

bool ServiceManager::nameIsWellformed(const QString name) const
{
	OC_METHODGATE();
	const auto trimmed=name.trimmed();
	if(trimmed.isEmpty()) {
		qWarning()<<"WARNING: Name '"<<name<<"' was empty";
		return false;
	}
	if(trimmed!=name) {
		qWarning()<<"WARNING: Name '"<<name<<"' contained space in the ends";
		return false;
	}
	return true;
}


bool ServiceManager::registered(const QString name) const
{
	OC_METHODGATE();
	auto it=mServicesMap.find(name);
	return(mServicesMap.constEnd() != it);
}


bool ServiceManager::registered(const QSharedPointer <Service> service) const
{
	OC_METHODGATE();
	if(service.isNull()) {
		qDebug()<<"WARNING: Trying to see if null service existed";
		return false;
	}
	return mServicesList.contains(service);
}


bool ServiceManager::activatedWanted(const QString name) const
{
	OC_METHODGATE();
	auto service=serviceByName(name);
	if(!service.isNull()) {
		return service->serviceActiveWanted();
	}
	return false;
}



bool ServiceManager::activatedActual(const QString name) const
{
	OC_METHODGATE();
	auto service=serviceByName(name);
	if(!service.isNull()) {
		return service->serviceActiveActual();
	}
	return false;
}

int ServiceManager::count() const
{
	OC_METHODGATE();
	return mServicesMap.count();
}


QSharedPointer <Service> ServiceManager::serviceByName(const QString name) const
{
	OC_METHODGATE();
	auto it=mServicesMap.find(name);
	if(mServicesMap.constEnd() != it) {
		return it.value();
	}
	return QSharedPointer<Service>();
}


QSet<QString> ServiceManager::dependencies(const QString name, const bool recurse) const
{
	OC_METHODGATE();
	QSet<QString> ret;
	auto service=serviceByName(name);
	if(!service.isNull()) {
		QStringList deps=service->dependencies();
		for(auto dep:deps) {
			//qDebug()<<name<<"-DEP: "<<dep;
			if(recurse) {
				auto sub=dependencies(dep, recurse);
				ret+=sub;
			}
			ret+=dep;
		}
	}
	return ret;
}


QSet<QString> ServiceManager::dependencies(const QSharedPointer <Service> service) const
{
	OC_METHODGATE();
	if(registered(service)) {
		auto name=service->name();
		// TODO: Really should be other way around, the name one should call the service one
		return dependencies(name);
	} else {
		qWarning()<<"ERROR: Trying to dependencies of unregistered service";
		return QSet<QString>();
	}
}


QSet<QString> ServiceManager::dependents(const QString name) const
{
	OC_METHODGATE();
	QSet<QString> ret;
	auto service=serviceByName(name);
	if(!service.isNull()) {
		for(auto one:mServicesMap.keys()) {
			//qDebug()<<name<<"-DEP: "<<dep;
			auto deps=dependencies(one);
			if(deps.constEnd()!=deps.find(name)) {
				ret+=one;
			}
		}
	}
	return ret;
}


QSet<QString> ServiceManager::dependents(const QSharedPointer <Service> service) const
{
	OC_METHODGATE();
	if(registered(service)) {
		auto name=service->name();
		// TODO: Really should be other way around, the name one should call the service one
		return dependents(name);
	} else {
		qWarning()<<"ERROR: Trying to dependents of unregistered service";
		return QSet<QString>();
	}
}


bool ServiceManager::met(QSet<QString> set, const bool met, const bool actual) const
{
	OC_METHODGATE();
	for(auto dep:set) {
		if((actual?activatedActual(dep):activatedWanted(dep))!=met) {
			return false;
		}
	}
	return true;
}


QSet<QString> ServiceManager::metSet(QSet<QString> set, const bool met, const bool actual) const
{
	OC_METHODGATE();
	QSet<QString> ret;
	for(auto dep:set) {
		if((actual?activatedActual(dep):activatedWanted(dep))!=met) {
			ret+=dep;
		}
	}
	return ret;
}


QSet<QString> ServiceManager::all() const
{
	OC_METHODGATE();
	auto list=mServicesMap.keys();
	return QSet<QString>(list.begin(), list.end());
}


QSet<QString> ServiceManager::all(const bool on, const bool  actual) const
{
	OC_METHODGATE();
	QSet<QString> ret;
	auto list=mServicesMap.keys();
	QSet<QString> all(list.begin(), list.end());
	for(auto name:all) {
		if((actual?activatedActual(name):activatedWanted(name))==on) {
			ret+=name;
		}
	}
	return ret;
}


void ServiceManager::dump() const
{
	OC_METHODGATE();
	qDebug()<<"SERVICES ";
	for(auto service:mServicesList) {
		qDebug()<<" + SERVICE: name="<<service->name()<<", activated=("<<service->serviceActiveWanted()<<","<<service->serviceActiveActual()<<"), dependencies="<<service->dependencies();
	}
}

QSet<QString> ServiceManager::expand(const QSet<QString> set) const
{
	OC_METHODGATE();
	QSet<QString> ret;
	for(auto name:set) {
		ret+=name;
		auto deps=dependencies(name);
		ret+=deps;
	}
	return ret;
}


bool ServiceManager::registerService(const QSharedPointer <Service> service)
{
	OC_METHODGATE();
	if(!service.isNull()) {
		const auto name=service->name();
		if(nameIsWellformed(name)) {
			if(!registered(service)) {
				mServicesMap[name]=service;
				mServicesList << service;
				emit servicesChanged();
				return true;
			} else {
				qWarning()<<"ERROR: Trying to double register service with name '"<<name<<"'";
			}
		} else {
			qWarning()<<"ERROR: Service had malformed name '"<<name<<"'";
		}
	} else {
		qWarning()<<"ERROR: Trying to register null service";
	}
	return false;
}


bool ServiceManager::unRegisterService(const QSharedPointer <Service> service)
{
	OC_METHODGATE();
	if(!service.isNull()) {
		auto name=service->name();
		return unRegisterService(name);
	} else {
		qWarning()<<"ERROR: Trying to un-register null service";
		return false;
	}
}


bool ServiceManager::unRegisterService(const QString name)
{
	OC_METHODGATE();
	const auto trimmed=name.trimmed();
	auto it=mServicesMap.find(name);
	if(mServicesMap.constEnd() != it) {
		mServicesMap.remove(name);
		mServicesList.removeAll(it.value());
		emit servicesChanged();
		return true;
	} else {
		qWarning()<<"ERROR: Trying to un-register non-registered service with name '"<<name<<"'";
	}
	return false;
}


bool ServiceManager::dependenciesMet(const QSharedPointer <Service> service) const
{
	OC_METHODGATE();
	if(registered(service)) {
		auto name=service->name();
		return dependenciesMet(name);
	} else {
		qWarning()<<"ERROR: Trying to check if unregistered service had dependencies met";
		return false;
	}
}


bool ServiceManager::dependenciesMet(const QString name) const
{
	OC_METHODGATE();
	bool ret(false);
	if(registered(name)) {
		ret=true;
		QSet<QString> deps=dependencies(name);
		for(auto dep:deps) {
			if(!activatedWanted(dep)) {
				ret=false;
				break;
			}
		}
	}
	return ret;
}


bool ServiceManager::dependentsMet(const QSharedPointer <Service> service) const
{
	OC_METHODGATE();
	if(registered(service)) {
		auto name=service->name();
		return dependentsMet(name);
	} else {
		qWarning()<<"ERROR: Trying to check if unregistered service had dependents met";
		return false;
	}
}


bool ServiceManager::dependentsMet(const QString name) const
{
	OC_METHODGATE();
	bool ret(false);
	if(registered(name)) {
		ret=true;
		QSet<QString> deps=dependents(name);
		for(auto dep:deps) {
			if(!activatedWanted(dep)) {
				ret=false;
				break;
			}
		}
	}
	return ret;
}


QSet<QString> ServiceManager::nextActivatableInSet(const QSet<QString> set, const bool active) const
{
	OC_METHODGATE();
	//qDebug()<<"nextActivatableInSet: set="<<set<<", active="<<active;
	QSet<QString> ret;
	for(auto name:set) {
		//qDebug()<<" + NAME: "<<name;
		if(active != activatedWanted(name)) {
			if(active) {
				auto dependenciesSet = dependencies(name);
				//qDebug()<<"   + DEPENDENCIES: "<<dependenciesSet;
				if(dependenciesMet(name)) {
					//qDebug()<<"     + MET";
					ret += name;
				} else {
					//qDebug()<<"     + UNMET";
					ret += nextActivatableInSet(dependenciesSet, active);
				}
			} else {
				auto dependentsSet = dependents(name);
				//qDebug()<<"   + DEPENDENTS: "<<dependentsSet;
				if(dependentsMet(name)) {
					//qDebug()<<"     + MET";
					ret += name;
				} else {
					//qDebug()<<"     + UNMET";
					ret += nextActivatableInSet(dependentsSet, active);
				}
			}
		}
	}
	//qDebug()<<"   + RET: "<<ret;
	return ret;
}


struct synchronizer {
	int total;
	int left;
	//bool gok;

	synchronizer():total(0), left(0)//, gok(true)
	{

	}
	void inc()
	{
		total++;
		left++;
	}
	bool end(bool ok)
	{
		Q_UNUSED(ok);
		//gok=gok&ok;
		left--;
		return left<=0;
	}

};


void ServiceManager::changeActivation(const QSet<QString> activateSet, const QSet<QString> deactivateSet, ServiceManagerCallback callBack)
{
	OC_METHODGATE();
	//qDebug().noquote().nospace()<<"changeActivation called with activate="<<activateSet<<" deactivate="<<deactivateSet;
	auto workingActivate = nextActivatableInSet(activateSet, true);
	auto workingDeactivate = nextActivatableInSet(deactivateSet, false);
	// Make sure that we don't accidentally deactivate services we actually need.
	workingDeactivate -= workingActivate;
	// Create one big set of all serviecs that will possibly change
	auto changing = workingDeactivate + workingActivate;

	auto sync = OC_NEW synchronizer;
	if(!changing.isEmpty()) {
		// qDebug()<<" -- Changing service set: "<<changing;
		for(const auto &name:changing) {
			auto service = serviceByName(name);
			if(!service.isNull()) {
				const bool currentlyWanted = service->serviceActiveWanted();
				const bool wanted = workingActivate.contains(name);
				// Is there a change?
				if(currentlyWanted != wanted) {
					// qDebug().noquote().nospace()<<" --- In change set: "<<name<<" for "<<(wanted?"ACTIVATION":"DEACTIVATION");
					sync->inc();
				}
			} else {
				qWarning() << "WARNING: Found invalid service: " << name;
			}
		}
		for(const auto &name:changing) {
			auto service = serviceByName(name);
			if(!service.isNull()) {
				const bool currentlyWanted = service->serviceActiveWanted();
				const bool wanted = workingActivate.contains(name);
				// Is there a change?
				if(currentlyWanted != wanted) {
					// Recurse asynchronously
					//qDebug().noquote().nospace()<<" --- Change set: "<<name<<" for "<<(wanted?"ACTIVATION":"DEACTIVATION")<<" STARTED";
					service->serviceChangeActivation(wanted, [=](bool on, bool ok) {
						Q_UNUSED(on);
						//qDebug().noquote().nospace()<<" ---- Change set "<<name<<" for "<<(wanted?"ACTIVATION":"DEACTIVATION")<<" "<<(ok?"SUCCEEDED":"FAILED");
						if(sync->end(ok)) {
							// qDebug().noquote().nospace()<<" ------ Recursing at end: "<<name<<" with "<<active;
							delete sync;
							changeActivation(activateSet, deactivateSet, callBack);
						}
					});
				}
			} else {
				qWarning() << "WARNING: Found invalid service: " << name;
			}
		}
	} else {
//		const bool gok=s->gok;
		//qDebug()<<"Service change complete";
		if(nullptr!=callBack) {
			callBack(true);
			emit servicesChanged();
		}
	}
}






void ServiceManager::changeActivation(const QSet<QString> set, const bool active, ServiceManagerCallback callBack)
{
	OC_METHODGATE();
	// qDebug()<<"Activate called with: "<<set<<" and "<<active;
	QSet<QString> working=nextActivatableInSet(set, active);
	synchronizer *s=OC_NEW synchronizer;
	if(!working.isEmpty()) {
		qDebug()<<" -- Activating service set: "<<working<<" with "<<active;
		for(auto name:working) {
			auto service = serviceByName(name);
			if(!service.isNull()) {
				if(active!=service->serviceActiveWanted()) {
					s->inc();
				}
			} else {
				qWarning()<<"WARNING: Found invalid service: "<<name;
			}
		}

		for(auto name:working) {
			auto service = serviceByName(name);
			if(!service.isNull()) {
				if(active!=service->serviceActiveWanted()) {
					// Recurse asynchronously
					service->serviceChangeActivation(active, [=](bool on, bool ok) {
						Q_UNUSED(on);
						qDebug()<<" ---- Activating service : "<<name<<" with "<<active;
						if(s->end(ok)) {
							// qDebug()<<" ------ Recursing at end: "<<name<<" with "<<active;
							delete s;
							changeActivation(set, active, callBack);
						}
					});
				}
			} else {
				qWarning()<<"WARNING: Found invalid service: "<<name;
			}
		}


	} else {
//		const bool gok=s->gok;
		qDebug()<<"Service activation with "<<active<<" complete";
		if(nullptr!=callBack) {
			callBack(true);
			emit servicesChanged();
		}
	}
}


void ServiceManager::changeActivation(const QString service, const bool active, ServiceManagerCallback callBack)
{
	OC_METHODGATE();
	changeActivation(QSet<QString> {service}, active, callBack);
}

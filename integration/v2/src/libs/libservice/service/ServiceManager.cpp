#include "ServiceManager.hpp"

#include "Service.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QDebug>
#include <QSharedPointer>

bool ServiceManager::nameIsWellformed(const QString name)
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


bool ServiceManager::registered(const QString name)
{
	OC_METHODGATE();
	auto it=mServicesMap.find(name);
	return(mServicesMap.constEnd() != it);
}


bool ServiceManager::registered(const QSharedPointer <Service> service)
{
	OC_METHODGATE();
	if(service.isNull()) {
		qDebug()<<"WARNING: Trying to see if null service existed";
		return false;
	}
	return mServicesList.contains(service);
}


bool ServiceManager::activated(const QString name)
{
	OC_METHODGATE();
	auto service=serviceByName(name);
	if(!service.isNull()) {
		return service->serviceActivated();
	}
	return false;
}

int ServiceManager::count()
{
	OC_METHODGATE();
	return mServicesMap.count();
}


QSharedPointer <Service> ServiceManager::serviceByName(const QString name)
{
	OC_METHODGATE();
	auto it=mServicesMap.find(name);
	if(mServicesMap.constEnd() != it) {
		return it.value();
	}
	return QSharedPointer<Service>();
}


QSet<QString> ServiceManager::dependencies(const QString name)
{
	OC_METHODGATE();
	QSet<QString> ret;
	auto service=serviceByName(name);
	if(!service.isNull()) {
		QStringList deps=service->dependencies();
		for(auto dep:deps) {
			//qDebug()<<name<<"-DEP: "<<dep;
			auto sub=dependencies(dep);
			ret+=sub;
			ret+=dep;
		}
	}
	return ret;
}


QSet<QString> ServiceManager::dependencies(const QSharedPointer <Service> service)
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



QSet<QString> ServiceManager::dependents(const QString name)
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


QSet<QString> ServiceManager::dependents(const QSharedPointer <Service> service)
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


QSet<QString> ServiceManager::metDependencies(const QString name, const bool met)
{
	OC_METHODGATE();
	QSet<QString> all=dependencies(name);
	QSet<QString> ret;
	for(auto dep:all) {
		auto service = serviceByName(dep);
		if(!service.isNull()) {
			if(met==service->serviceActivated()) {
				ret+=dep;
			}
		} else {
			qWarning()<<"WARNING: Found invalid dependency: "<<dep;
			ret+=dep;
		}
	}
	return ret;
}

QSet<QString> ServiceManager::metDependents(const QString name, const bool met)
{
	OC_METHODGATE();
	QSet<QString> all=dependents(name);
	QSet<QString> ret;
	for(auto dep:all) {
		auto service = serviceByName(dep);
		if(!service.isNull()) {
			if(met==service->serviceActivated()) {
				ret+=dep;
			}
		} else {
			qWarning()<<"WARNING: Found invalid dependent: "<<dep;
			ret+=dep;
		}
	}
	return ret;
}

QSet<QString> ServiceManager::all()
{
	OC_METHODGATE();
	return mServicesMap.keys().toSet();
}

// Show internal state
void ServiceManager::dump()
{
	OC_METHODGATE();
	qDebug()<<"SERVICES ";
	for(auto service:mServicesList) {
		qDebug()<<" + SERVICE: name="<<service->name()<<", activated="<<service->serviceActivated()<<", dependencies="<<service->dependencies();
	}
}


bool ServiceManager::registerService(const QSharedPointer <Service> service)
{
	OC_METHODGATE();
	if(!service.isNull()) {
		const auto name=service->name();
		const auto trimmed=name.trimmed();
		if(nameIsWellformed(name)) {
			if(!registered(service)) {
				mServicesMap[name]=service;
				mServicesList << service;
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
		return true;
	} else {
		qWarning()<<"ERROR: Trying to un-register non-registered service with name '"<<name<<"'";
	}
	return false;
}


bool ServiceManager::dependenciesMet(const QSharedPointer <Service> service)
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


bool ServiceManager::dependenciesMet(const QString name)
{
	OC_METHODGATE();
	bool ret(false);
	if(registered(name)) {
		ret=true;
		QSet<QString> deps=dependencies(name);
		for(auto dep:deps) {
			if(!activated(dep)) {
				ret=false;
				break;
			}
		}
	}
	return ret;
}


bool ServiceManager::dependentsMet(const QSharedPointer <Service> service)
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


bool ServiceManager::dependentsMet(const QString name)
{
	OC_METHODGATE();
	bool ret(false);
	if(registered(name)) {
		ret=true;
		QSet<QString> deps=dependents(name);
		for(auto dep:deps) {
			if(!activated(dep)) {
				ret=false;
				break;
			}
		}
	}
	return ret;
}


QSet<QString> ServiceManager::nextActivatableInSet(const QSet<QString> set, const bool active)
{
	OC_METHODGATE();
	//qDebug()<<"nextActivatableInSet: set="<<set<<", active="<<active;
	QSet<QString> ret;
	for(auto name:set) {
		//qDebug()<<" + NAME: "<<name;
		if(active!=activated(name)) {
			if(active) {
				auto dependenciesSet=dependencies(name);
				//qDebug()<<"   + DEPENDENCIES: "<<dependenciesSet;
				if(dependenciesMet(name)) {
					//qDebug()<<"     + MET";
					ret+=name;
				} else {
					//qDebug()<<"     + UNMET";
					ret+=nextActivatableInSet(dependenciesSet, active);
				}
			} else {
				auto dependentsSet=dependents(name);
				//qDebug()<<"   + DEPENDENTS: "<<dependentsSet;
				if(dependentsMet(name)) {
					//qDebug()<<"     + MET";
					ret+=name;
				} else {
					//qDebug()<<"     + UNMET";
					ret+=nextActivatableInSet(dependentsSet, active);
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
	bool gok;

	synchronizer():total(0), left(0), gok(true)
	{

	}
	void start()
	{
		total++;
		left++;
	}
	bool end(bool ok)
	{
		gok=gok&ok;
		left--;
		return left<=0;
	}

};


void ServiceManager::activate(const QSet<QString> set, const bool active, ServiceActivatedCallback callBack)
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
				if(active!=service->serviceActivated()) {
					s->start();
				}
			} else {
				qWarning()<<"WARNING: Found invalid service: "<<name;
			}
		}

		for(auto name:working) {
			auto service = serviceByName(name);
			if(!service.isNull()) {
				if(active!=service->serviceActivated()) {
					// Recurse asynchronously
					service->serviceActivate(active, [=](bool ok) {
						qDebug()<<" ---- Activating service : "<<name<<" with "<<active;
						if(s->end(ok)) {
							// qDebug()<<" ------ Recursing at end: "<<name<<" with "<<active;
							delete s;
							activate(set, active, callBack);
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
		callBack(true);
	}
}


void ServiceManager::activate(const QString service, const bool active, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	activate(QSet<QString>{service}, active, callBack);
}

#include "ServiceLevelManager.hpp"

#include "ServiceLevel.hpp"
#include "ServiceManager.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/New.hpp"

#include <QDebug>

ServiceLevelManager::ServiceLevelManager()
	: QObject (nullptr)
	, mConfigureHelper("ServiceLevelManager", true, false, false, true, false)
{
	OC_METHODGATE();
}


ServiceLevelManager::~ServiceLevelManager()
{
	OC_METHODGATE();
}


void ServiceLevelManager::configure()
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mServiceManager=QSharedPointer<ServiceManager>(OC_NEW ServiceManager());
		if(!mServiceManager.isNull()) {
			// Forward servicvesChanged signal
			ServiceManager *sm=mServiceManager.data();
			if(!connect(sm, &ServiceManager::servicesChanged, this, &ServiceLevelManager::servicesChanged, OC_CONTYPE)) {
				qWarning()<<"ERROR: Could not connect";
			}
		} else {
			qWarning()<<"ERROR: No mServiceManager";
		}
	}
}


QSharedPointer <Service> ServiceLevelManager::serviceByName(const QString name) const
{
	OC_METHODGATE();
	QSharedPointer <Service> service;
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mServiceManager.isNull()) {
			service=mServiceManager->serviceByName(name);
		} else {
			qWarning()<<"ERROR: No service manager while looking up service by name";
		}
	}
	return service;
}


QSharedPointer <ServiceLevel> ServiceLevelManager::serviceLevelByName(const QString name) const
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		auto it=mServiceLevelsMap.find(name);
		if(mServiceLevelsMap.constEnd() != it) {
			return it.value();
		}
	}
	return nullptr;
}


QSharedPointer<ServiceManager> ServiceLevelManager::serviceManager()
{
	OC_METHODGATE();
	return mServiceManager;
}


bool ServiceLevelManager::isServiceRegistered(const QString name) const
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mServiceManager.isNull()) {
			return mServiceManager->registered(name);
		} else {
			qWarning()<<"ERROR: No service manager while registering service";
		}
	}
	return false;
}

bool ServiceLevelManager::isServiceRegistered(const QSharedPointer <Service> serviceByName) const
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mServiceManager.isNull()) {
			return mServiceManager->registered(serviceByName);
		} else {
			qWarning()<<"ERROR: No service manager while registering service";
		}
	}
	return false;
}

bool ServiceLevelManager::isServiceLevelRegistered(const QString name) const
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		auto it=mServiceLevelsMap.find(name);
		return(mServiceLevelsMap.constEnd() != it);
	}
	return false;
}

bool ServiceLevelManager::isServiceLevelRegistered(const QSharedPointer <ServiceLevel> serviceLevelByName) const
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(serviceLevelByName.isNull()) {
			qWarning()<<"WARNING: Trying to see if null service level existed";
			return false;
		}
		return mServiceLevelsList.contains(serviceLevelByName);

	}
	return false;
}

QSet<QString> ServiceLevelManager::wantedServices() const
{
	OC_METHODGATE();
	QSet<QString> ret;
	for(auto serviceLevel:mServiceLevelsList) {
		if(!serviceLevel.isNull()) {
			if(serviceLevel->enabled()) {
				ret+=serviceLevel->serviceNames();
			}
		} else {
			qWarning()<<"WARNING: Invalid service level found while collecting wanted services";
		}
	}
	return ret;
}


bool ServiceLevelManager::registerService(const QSharedPointer <Service> service)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mServiceManager.isNull()) {
			return mServiceManager->registerService(service);
		} else {
			qWarning()<<"ERROR: No service manager while registering service";
		}
	}
	return false;
}


bool ServiceLevelManager::unRegisterService(const QSharedPointer <Service> service)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mServiceManager.isNull()) {
			return mServiceManager->unRegisterService(service);
		} else {
			qWarning()<<"ERROR: No service manager while unregistering service";
		}
	}
	return false;
}


bool ServiceLevelManager::unRegisterService(const QString name)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mServiceManager.isNull()) {
			return mServiceManager->unRegisterService(name);
		} else {
			qWarning()<<"ERROR: No service manager while unregistering service by name";
		}

	}
	return false;
}


bool ServiceLevelManager::registerServiceLevel(QSharedPointer<ServiceLevel> serviceLevel)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!serviceLevel.isNull()) {
			const auto name=serviceLevel->name();
			if(!isServiceLevelRegistered(serviceLevel)) {
				mServiceLevelsMap[name]=serviceLevel;
				mServiceLevelsList << serviceLevel;
				return true;
			} else {
				qWarning()<<"ERROR: Trying to double register service with name '"<<name<<"'";
			}

		} else {
			qWarning()<<"ERROR: Trying to register null service";
		}
	}
	return false;
}


void ServiceLevelManager::synchronizeServiceManager(const std::function<void(const bool ok)> callBack)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mServiceManager.isNull()) {
			// qDebug().noquote().nospace()<<"**** synchronizeServiceManager ****";
			// Get the services we want to enable
			QSet<QString> wantedRaw=wantedServices();
			// qDebug().noquote().nospace()<<"**** synchronizeServiceManager new wantedRaw="<<wantedRaw;
			// Expand to include dependencies
			QSet<QString> wanted=mServiceManager->expand(wantedRaw);
			// qDebug().noquote().nospace()<<"**** synchronizeServiceManager new wanted="<<wanted;
			// Get the existing services
			QSet<QString> currentlyWanted=mServiceManager->all(true, false);
			// qDebug().noquote().nospace()<<"**** synchronizeServiceManager currentlyWanted="<<currentlyWanted;
			// Get the services we want to disable
			QSet<QString> unwanted=currentlyWanted-wanted;
			// qDebug().noquote().nospace()<<"**** synchronizeServiceManager unwanted="<<unwanted;
			// Cull all services that are already wanted
			wanted-=currentlyWanted;
			// qDebug().noquote().nospace()<<"**** synchronizeServiceManager wanted="<<wanted<<" unwanted="<<unwanted;
			// Apply the new recommendation
			mServiceManager->changeActivation(wanted, unwanted, [this, callBack](bool ok) {
				//qDebug()<<"POLKA=" << ok;
				if(nullptr!= callBack) {
					callBack(ok);
				}
				emit servicesChanged();
			});
		} else {
			qWarning()<<"ERROR: No service manager while synchronizing service manager";
		}
	}
}


void ServiceLevelManager::enableLevel(const QString name, const bool on, const std::function<void(const bool ok)> callBack)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		enableLevels({name}, on, callBack);
	}
}



void ServiceLevelManager::enableLevels(const QSet<QString> names, const bool on, const std::function<void(const bool ok)> callBack)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		int changed=0;
		for(auto name:names) {
			auto serviceLevel=serviceLevelByName(name);
			if(!serviceLevel.isNull()) {
				//qDebug()<<"Setting service level "<<name<<" with"<<serviceLevel->serviceNames()<<" to "<<(on?"ACTIVE":"INACTIVE");
				serviceLevel->enable(on);
				changed++;
			} else {
				qWarning()<<"ERROR: No service level for name '"<<name<<"'";
			}
		}
		if(changed>0) {
			synchronizeServiceManager(callBack);
		}
	}
}


void ServiceLevelManager::onServiceLevelChanged()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		synchronizeServiceManager();
	}
}

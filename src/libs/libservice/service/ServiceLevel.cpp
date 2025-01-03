#include "ServiceLevel.hpp"

#include "uptime/MethodGate.hpp"

ServiceLevel::ServiceLevel(QString name, QSet<QString> serviceNames)
	: QObject (nullptr)
	, mName(name)
	, mServiceNames(serviceNames)
	, mEnableWanted(false)
{
	OC_METHODGATE();
}


ServiceLevel::~ServiceLevel()
{
	OC_METHODGATE();
}


bool ServiceLevel::enabled() const
{
	OC_METHODGATE();
	return mEnableWanted;
}


QString ServiceLevel::name() const
{
	OC_METHODGATE();
	return mName;
}


QSet<QString> ServiceLevel::serviceNames() const
{
	OC_METHODGATE();
	return mServiceNames;
}

void ServiceLevel::toggleService(const QString &name, bool wantService){
	OC_METHODGATE();
	const auto hadService = (mServiceNames.contains(name));
	if(hadService != wantService){
		if(wantService){
			mServiceNames.insert(name);
		}
		else{
			mServiceNames.remove(name);
		}
	}
}


void ServiceLevel::enable(const bool wanted)
{
	OC_METHODGATE();
	mEnableWanted = wanted;
	emit managerChange(wanted);
}


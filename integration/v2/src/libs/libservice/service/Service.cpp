#include "Service.hpp"

#include "uptime/MethodGate.hpp"

Service::Service(QString name, QStringList dependencies)
	: mName(name)
	, mDependencies(dependencies)
{
	OC_METHODGATE();
}

Service::~Service()
{
	OC_METHODGATE();
}

QString Service::name()
{
	OC_METHODGATE();
	return mName;
}

QStringList Service::dependencies()
{
	OC_METHODGATE();
	return mDependencies;
}




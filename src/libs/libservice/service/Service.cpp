#include "Service.hpp"

#include "uptime/MethodGate.hpp"

Service::Service(QString name, QStringList dependencies)
	: mName(name)
	, mDependencies(dependencies)
	, mActiveWanted(false)
	, mActive(false)
{
	OC_METHODGATE();
}


Service::~Service()
{
	OC_METHODGATE();
}


QString Service::name() const
{
	OC_METHODGATE();
	return mName;
}


QStringList Service::dependencies() const
{
	OC_METHODGATE();
	return mDependencies;
}


void Service::serviceChangeActivation(bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	mActiveWanted=on;
	serviceActivateImp(on, [this, callBack](bool on, bool ok) {
		mActive=ok?on:mActive;
		if(nullptr!=callBack) {
			callBack(on, ok);
		}
	});
}


bool Service::serviceActiveWanted() const
{
	OC_METHODGATE();
	return mActiveWanted;
}


bool Service::serviceActiveActual() const
{
	OC_METHODGATE();
	return mActive;
}


void Service::setRegistrationOrder(int order){
	OC_METHODGATE();
	mRegistrationOrder = order;
}


int Service::registrationOrder(){
	OC_METHODGATE();
	return mRegistrationOrder;
}

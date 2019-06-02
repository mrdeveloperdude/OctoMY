#include "LocalIdentityStore.hpp"

#include "uptime/MethodGate.hpp"

LocalIdentityStore::LocalIdentityStore()
	: mConfigureHelper("LocalIdentityStore", true, false, false, true, false)
{
	OC_METHODGATE();
}


LocalIdentityStore::~LocalIdentityStore()
{
	OC_METHODGATE();
}


void LocalIdentityStore::configure(QString filename)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		SimpleMapStore::configure(filename);
	}
}

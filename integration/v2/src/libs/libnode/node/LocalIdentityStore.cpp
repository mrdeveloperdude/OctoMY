#include "LocalIdentityStore.hpp"

#include "uptime/MethodGate.hpp"

LocalIdentityStore::LocalIdentityStore()
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
	SimpleMapStore::configure(filename);
}

#include "node/LocalIdentityStore.hpp"

LocalIdentityStore::LocalIdentityStore(QString filename)
	: SimpleMapStore(filename)
{
	OC_METHODGATE();
}


LocalIdentityStore::~LocalIdentityStore()
{
	OC_METHODGATE();
}

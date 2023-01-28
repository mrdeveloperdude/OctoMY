#include "node/LocalIdentityStore.hpp"

#include "utility/Standard.hpp"

LocalIdentityStore::LocalIdentityStore(QString filename)
	: SimpleMapStore(filename)
{
	OC_METHODGATE();
}


LocalIdentityStore::~LocalIdentityStore()
{
	OC_METHODGATE();
}

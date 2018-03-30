#ifndef LOCALIDENTITYSTORE_HPP
#define LOCALIDENTITYSTORE_HPP

#include "DataStore.hpp"


class LocalIdentityStore: public SimpleMapStore
{
public:
	explicit LocalIdentityStore(QString filename="");
	virtual ~LocalIdentityStore();
};

#endif // LOCALIDENTITYSTORE_HPP

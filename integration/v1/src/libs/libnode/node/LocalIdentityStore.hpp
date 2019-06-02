#ifndef LOCALIDENTITYSTORE_HPP
#define LOCALIDENTITYSTORE_HPP

#include "node/SimpleMapStore.hpp"


class LocalIdentityStore: public SimpleMapStore
{
public:
	explicit LocalIdentityStore(QString filename="");
	virtual ~LocalIdentityStore();
};

#endif // LOCALIDENTITYSTORE_HPP

#ifndef LOCALIDENTITYSTORE_HPP
#define LOCALIDENTITYSTORE_HPP

#include "store/SimpleMapStore.hpp"


class LocalIdentityStore: public SimpleMapStore
{
public:
	explicit LocalIdentityStore();
	virtual ~LocalIdentityStore();

public:
	void configure(QString filename);
};

#endif
// LOCALIDENTITYSTORE_HPP

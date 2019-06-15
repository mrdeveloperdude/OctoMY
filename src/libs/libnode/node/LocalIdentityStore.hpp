#ifndef LOCALIDENTITYSTORE_HPP
#define LOCALIDENTITYSTORE_HPP

#include "uptime/ConfigureHelper.hpp"
#include "store/SimpleMapStore.hpp"

/**
 * @brief The LocalIdentityStore class keeps the identity of the node safe.
 * It uses SimpleMapStore to persist a map containing the identity of the node.
 * The identity is in associate format, please see Associate class for details.
 *
 * The relationship between KeyStore and LocalIdentityStore is such that:
 *  + KeyStore will only maintain the privatekey and no other data for node, LocalIdentityStore maintains all identity data such as name and gender.
 *  + KeyStore will maintain the private key, only id is stored in LocalIdentityStore
 *  + KeyStore can create new keys and operate on them
 *  + It is node's job to listen for changes to local key in keystore and propegate them to localidentiy store
 *
 * The relationship between AddressBook and LocalIdentityStore is such that:
 *  + LocalIdentityStore will maintain only the node's local identity
 *  + AddressBook will maintain associate's identities but not the local identity
 *  + Both LocalIdentityStore and AddressBook are services. They are completely independent
 *
 * The relationship between KeyStore and AddressBook is such that:
 *  + KeyStore will only maintain the pubkey and no other data for each associate, AddressBook maintains all identity data such as name and gender.
 *  + KeyStore will maintain the pubkey, only id is stored in AddressBook
 *  + It is node's job to listen for changes to associate's keys in keystore and propegate them to localidentiy store
 *
 * See also Associate.hpp, KeyStore.hpp, AddressBook.hpp
 */
class LocalIdentityStore: public SimpleMapStore
{
private:
	ConfigureHelper mConfigureHelper;

public:
	explicit LocalIdentityStore();
	virtual ~LocalIdentityStore();

public:
	void configure(QString filename);

};

#endif
// LOCALIDENTITYSTORE_HPP

#ifndef LOCALIDENTITYSTORESERVICE_HPP
#define LOCALIDENTITYSTORESERVICE_HPP

#include "node/LocalIdentityStore.hpp"
#include "service/ServiceWrapper.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QSharedPointer>
#include <QVariantMap>

class Node;

/**
 * @brief The LocalIdentityStoreService class is what it says on the tin; basically a wrapper to make LocalIdentityStore appear as a service.
 */

class LocalIdentityStoreService: public ServiceWrapper<LocalIdentityStore>
{
private:
	QSharedPointer<Node> mNode;
	bool mDebug{false};
	ConfigureHelper mConfigureHelper;

public:
	LocalIdentityStoreService(QSharedPointer<LocalIdentityStore> localIdentityStore, QStringList dependencies= {"KeyStore"});

public:
	void configure(QSharedPointer<Node> node);

	// ServiceWapper interface.
public:
	void serviceWrapperActivate(QSharedPointer<LocalIdentityStore> wrapee, bool on, ServiceActivatedCallback callBack) override;

};



#endif
// LOCALIDENTITYSTORESERVICE_HPP

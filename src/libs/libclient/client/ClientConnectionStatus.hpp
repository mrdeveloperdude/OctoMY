#ifndef CLIENTCONNECTIONSTATUS_HPP
#define CLIENTCONNECTIONSTATUS_HPP

#include "comms/IConnectionStatus.hpp"

#include "uptime/SharedPointerWrapper.hpp"

class Client;

class ClientConnectionStatus: public IConnectionStatus
{
private:
	QSharedPointer<Client> mClient;
public:
	ClientConnectionStatus(QSharedPointer<Client>);
	virtual ~ClientConnectionStatus() override {}


	// IConnectionStatus interface
public:
	bool needsConnection() override;
	bool isConnected() override;
	void setNeedsConnection(bool) override;
	void setConnected(bool) override;

};

#endif
// CLIENTCONNECTIONSTATUS_HPP

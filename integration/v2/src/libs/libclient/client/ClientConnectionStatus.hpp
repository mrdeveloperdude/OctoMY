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


	// IConnectionStatus interface
public:
	bool needsConnection() Q_DECL_OVERRIDE;
	bool isConnected() Q_DECL_OVERRIDE;
	void setNeedsConnection(bool) Q_DECL_OVERRIDE;
	void setConnected(bool) Q_DECL_OVERRIDE;

};

#endif
// CLIENTCONNECTIONSTATUS_HPP

#ifndef UDPTESTER_HPP
#define UDPTESTER_HPP

#include "comms/NetworkAddress.hpp"

#include <QUdpSocket>

class UDPTester
{
private:
	NetworkAddress mFromAddr;
	NetworkAddress mToAddr;
	QUdpSocket mSocket;

public:
	UDPTester(NetworkAddress fromAddr, NetworkAddress toAddr);

public:
	qint64 send(QByteArray data);
	QByteArray receive();

};



#endif // UDPTESTER_HPP

#ifndef UDPTESTER_HPP
#define UDPTESTER_HPP

#ifdef USE_UDP_TESTER

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

	qint64 send(QByteArray data);
	QByteArray receive();
};


#endif
// USE_UDP_TESTER

#endif // UDPTESTER_HPP

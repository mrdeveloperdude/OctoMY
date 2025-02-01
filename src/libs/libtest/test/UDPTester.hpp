#ifndef UDPTESTER_HPP
#define UDPTESTER_HPP

#include "uptime/SharedPointerWrapper.hpp"

#include <QUdpSocket>

class CarrierAddressUDP;


class UDPTester
{
private:
	QSharedPointer<CarrierAddressUDP> mFromAddr;
	QSharedPointer<CarrierAddressUDP> mToAddr;
	QUdpSocket mSocket;

public:
	UDPTester(QSharedPointer<CarrierAddressUDP> fromAddr, QSharedPointer<CarrierAddressUDP> toAddr);

public:
	qint64 send(QByteArray data);
	QByteArray receive();

};



#endif // UDPTESTER_HPP

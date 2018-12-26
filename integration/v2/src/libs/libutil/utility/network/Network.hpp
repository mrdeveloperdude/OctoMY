#ifndef UTILITY_NETWORK_HPP
#define UTILITY_NETWORK_HPP


#include <QList>

class QHostAddress;


namespace utility
{



namespace network
{

QList<QHostAddress> allLocalNetworkAddresses();
QString localAddress();
bool checkUDPAddress(QHostAddress address, quint16 port);
quint16	freeUDPPortForAddress(QHostAddress adr);
QHostAddress defaultGatewayAddress();

quint32 addressCloseness(QHostAddress a, QHostAddress b);
QHostAddress closestAddress(QList<QHostAddress> addresses, QHostAddress network);
bool isValidIPv4(const QString str);
bool isValidIPv6(const QString str);
bool isValidIPv4(const QHostAddress addr);
bool isValidIPv6(const QHostAddress addr);


}

}


#endif
// UTILITY_NETWORK_HPP


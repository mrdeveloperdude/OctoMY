#include "utility/network/Network.hpp"

#include <QHostAddress>
#include <QNetworkInterface>
#include <QUdpSocket>
#include <QRegularExpression>

namespace utility
{

namespace network
{


QList<QHostAddress> allLocalCarrierAddresses()
{
	QList<QHostAddress> out;
	QList<QNetworkInterface> list= QNetworkInterface::allInterfaces();
	for(int i=0; i<list.size(); i++) {
		QNetworkInterface inter = list.at(i);
		if (inter.flags().testFlag(QNetworkInterface::IsUp) && !inter.flags().testFlag(QNetworkInterface::IsLoopBack)) {
			QList<QNetworkAddressEntry> list2= inter.addressEntries();
			for(int j=0; j<list2.size(); j++) {
				QNetworkAddressEntry entry = list2.at(j);
				out << entry.ip();

			}
		}
	}
	return out;
}

QString localAddress()
{
	QList<QHostAddress> list=allLocalCarrierAddresses();
	for(QHostAddress adr:list) {
		if((QAbstractSocket::IPv4Protocol==adr.protocol()) && (!adr.isLoopback()) ) {
			return adr.toString();
		}
	}
	return "127.0.0.1";
}

bool checkUDPAddress(QHostAddress address, quint16 port)
{
	// Avoid special case where port=0 returning true by mistake
	if(0 == port) {
		return false;
	}
	// Try the address+port by temporarily binding with it
	QUdpSocket udp;
	const bool ok=udp.bind(address, port);
	udp.abort();
	return ok;
}




quint16	freeUDPPortForAddress(QHostAddress address)
{
	QUdpSocket udp;
	// NOTE: If port==0, this will bind to a random available port (See documentation for QAbstractSocker::bind() for details )
	quint16 port=0;
	if(udp.bind(address, port)) {
		// Return whatever random available port was selected
		port = udp.localPort();
	}
	//qDebug()<<"Free UDP port "<<port<<" found for "<<address;
	return port;
}


// NOTE: "QHostAddress defaultGatewayAddress()" is in its separate file "DefaultGatewayUtil.cpp"
quint32 addressCloseness(QHostAddress a, QHostAddress b)
{
	// TODO: This is a grose hack that will probably fail spectacularly for any kind of IPv6
	const QString as=a.toString();
	const QString bs=b.toString();
	const quint32 len=static_cast<quint32>(qMin(as.size(), bs.size()));
	quint32 i=0;
	while(i<len && as[i]==bs[i]) {
		++i;
	}
	return i;
}

QHostAddress closestAddress(QList<QHostAddress> addresses, QHostAddress network)
{
	QHostAddress closest;
	quint32  closestMatch=0;
	for(QHostAddress address:addresses) {
		quint32 closeness=addressCloseness(address, network);
		if(closeness > closestMatch) {
			closestMatch = closeness;
			closest = address;
		}
	}
	return closest;
}


static const QRegularExpression reIPv4("(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])(?:\\.(?:25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])){3}");
static const QRegularExpression reIPv6("((?:[0-9a-fA-F]{1,4}:){7}(?:[0-9a-fA-F]{1,4}|:)|(?:[0-9a-fA-F]{1,4}:){6}(?:${v4}|:[0-9a-fA-F]{1,4}|:)|(?:[0-9a-fA-F]{1,4}:){5}(?::${v4}|(:[0-9a-fA-F]{1,4}){1,2}|:)|(?:[0-9a-fA-F]{1,4}:){4}(?:(:[0-9a-fA-F]{1,4}){0,1}:${v4}|(:[0-9a-fA-F]{1,4}){1,3}|:)|(?:[0-9a-fA-F]{1,4}:){3}(?:(:[0-9a-fA-F]{1,4}){0,2}:${v4}|(:[0-9a-fA-F]{1,4}){1,4}|:)|(?:[0-9a-fA-F]{1,4}:){2}(?:(:[0-9a-fA-F]{1,4}){0,3}:${v4}|(:[0-9a-fA-F]{1,4}){1,5}|:)|(?:[0-9a-fA-F]{1,4}:){1}(?:(:[0-9a-fA-F]{1,4}){0,4}:${v4}|(:[0-9a-fA-F]{1,4}){1,6}|:)|(?::((?::[0-9a-fA-F]{1,4}){0,5}:${v4}|(?::[0-9a-fA-F]{1,4}){1,7}|:)))(%[0-9a-zA-Z]{1,})?");


//static const QRegularExpression reIPv4("/^(25[0-5]|2[0-4][0-9]|1?[0-9][0-9]{1,2})(\\.(25[0-5]|2[0-4][0-9]|1?[0-9]{1,2})){3}$/");
//static const QRegularExpression reIPv6("/^([0-9a-f]){1,4}(:([0-9a-f]){1,4}){7}$/i");

bool isValidIPv4(const QString str)
{
	return isValidIPv4(QHostAddress(str));
}

bool isValidIPv6(const QString str)
{
	return isValidIPv6(QHostAddress(str));
}

bool isValidIPv4(const QHostAddress addr)
{
	// TODO: Look at fesability of this. Are there any cornercases where this string conversion might result in valid address even thought the original is not valid?
	const QString str=addr.toString();
	if(str.trimmed().isEmpty()){
		return false;
	}
	return reIPv4.match(str).hasMatch();
}

bool isValidIPv6(const QHostAddress addr)
{
	// TODO: Look at fesability of this. Are there any cornercases where this string conversion might result in valid address even thought the original is not valid?
	const QString str=addr.toString();
	if(str.trimmed().isEmpty()){
		return false;
	}
	return reIPv6.match(str).hasMatch();
}


}

}

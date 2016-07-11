#ifndef NETWORKADDRESS_HPP
#define NETWORKADDRESS_HPP

#include <QHostAddress>
#include <QVariantMap>


struct NetworkAddress{
		QHostAddress ip;
		quint16 port;

	public:
		NetworkAddress(QVariantMap map)
			: ip(map["ip"].toString())
			, port(map["port"].toInt())
		{

		}

		NetworkAddress(QHostAddress ip, quint16 port)
			: ip(ip)
			, port(port)
		{

		}

		NetworkAddress()
			: port(0)
		{

		}

		QVariantMap toVariantMap()
		{
			QVariantMap map;
			map["ip"]=ip.toString();
			map["port"]=port;
			return map;
		}

		QString toString()
		{
			return ip.toString()+":"+QString::number(port);
		}

		bool isValid()
		{
			return ( (0!=port) && (!ip.isNull()) );
		}

		bool operator==(const NetworkAddress &o) const
		{
			return o.port==port && o.ip==ip;
		}

};

#endif // NETWORKADDRESS_HPP

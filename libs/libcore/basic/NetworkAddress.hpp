#ifndef NETWORKADDRESS_HPP
#define NETWORKADDRESS_HPP


#include <QHostAddress>
#include <QVariantMap>



class NetworkAddress{
	private:
		QHostAddress mIP;
		quint16 mPort;

	public:
		explicit NetworkAddress();
		NetworkAddress(const NetworkAddress &other);
		explicit NetworkAddress(QHostAddress ip, quint16 port);
		explicit NetworkAddress(QVariantMap map);
		virtual ~NetworkAddress();

	public:
		QHostAddress ip();
		quint16 port();
		void setPort(quint16 port);
		void setIP(QHostAddress ip);
		QVariantMap toVariantMap();
		QString toString() const;
		bool isValid(bool allowLoopback=true, bool allowMulticast=false);
		bool operator==(const NetworkAddress &o) const;
		bool operator!=(const NetworkAddress &o) const;

};

const QDebug &operator<<(QDebug &d, const NetworkAddress &na);

#endif // NETWORKADDRESS_HPP

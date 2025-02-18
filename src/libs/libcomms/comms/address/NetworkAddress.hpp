#ifndef NETWORKADDRESS_H
#define NETWORKADDRESS_H


#include <QHostAddress>
#include <QVariantMap>


class QDataStream;

class NetworkAddress
{
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
	QHostAddress ip() const;
	quint16 port() const;
	void setPort(quint16 port);
	void setIP(QHostAddress ip);
	QVariantMap toVariantMap() const;
	QString toString() const;
	void fromString(QString, bool allowOnlyAddress=true);
	bool isValid(bool allowLoopback=true, bool allowMulticast=false, bool allowIPv6=true) const;
	bool isIPv4() const;
	bool isIPv6() const;
	bool operator==(const NetworkAddress &o) const;
	bool operator!=(const NetworkAddress &o) const;

	void toStream(QDataStream &)const;
	void fromStream(QDataStream &);

	operator QString() const;
	operator QString();

};

const QDebug &operator<<(QDebug &d, const NetworkAddress &na);
QDebug &operator<<(QDebug &d, NetworkAddress &na);


QDataStream &operator<<(QDataStream &, const NetworkAddress &);
QDataStream &operator>>(QDataStream &, NetworkAddress &);


#endif
// NETWORKADDRESS_HPP


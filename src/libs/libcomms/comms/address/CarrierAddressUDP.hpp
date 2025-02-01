#ifndef CARRIERADDRESSUDP_HPP
#define CARRIERADDRESSUDP_HPP

#include <QHostAddress>
#include <QVariantMap>

#include "CarrierAddress.hpp"

class CarrierAddressUDP: public CarrierAddress
{
private:
	QHostAddress mIP;
	quint16 mPort;

public:
	explicit CarrierAddressUDP();
	CarrierAddressUDP(const CarrierAddressUDP &other);
	explicit CarrierAddressUDP(QHostAddress ip, quint16 port);
	explicit CarrierAddressUDP(QVariantMap map);
	virtual ~CarrierAddressUDP();
	
	
	// UDP/IP spesific
public:
	QHostAddress ip() const;
	quint16 port() const;
	void setPort(quint16 port);
	void setIP(QHostAddress ip);
	bool isValidIP(bool allowLoopback=true, bool allowMulticast=false, bool allowIPv6=true) const;
	bool isIPv4() const;
	bool isIPv6() const;
	
	
	// CarrierAddress Interface
public:
	bool isValid() const override;
	bool equal(const CarrierAddress &) const override;
	void toVariantMap(QVariantMap &map) const override;
	QVariantMap toVariantMap() const override;
	QString toString() const override;
	void fromString(QString, bool allowOnlyAddress=true) override;
	
	void toStream(QDataStream &) const override;
	void fromStream(QDataStream &) override;

};

#endif // CARRIERADDRESSUDP_HPP


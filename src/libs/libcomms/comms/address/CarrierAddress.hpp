#ifndef CARRIERADDRESS_HPP
#define CARRIERADDRESS_HPP

#include "CarrierAddressType.hpp"

#include <QHostAddress>
#include <QVariantMap>


class QDataStream;

class CarrierAddress
{
protected:
	const CarrierAddressType mType;
public:
	explicit CarrierAddress(const CarrierAddressType type);

	// General
public:
	bool operator==(const CarrierAddress &o) const;
	bool operator!=(const CarrierAddress &o) const;
	bool is(const CarrierAddressType &type) const;
	CarrierAddressType type() const;
	operator QString() const;
	operator QString();

	// CarrierAddress Interface
public:
	virtual bool isValid() const = 0;
	virtual bool equal(const CarrierAddress &) const = 0;
	virtual void toVariantMap(QVariantMap &map) const = 0;
	virtual QVariantMap toVariantMap() const = 0;
	virtual QString toString() const = 0;
	virtual void fromString(QString, bool allowOnlyAddress=true) = 0;
	
	virtual void toStream(QDataStream &) const = 0;
	virtual void fromStream(QDataStream &) = 0;

};

const QDebug &operator<<(QDebug &d, const CarrierAddress &na);
QDebug &operator<<(QDebug &d, CarrierAddress &na);


QDataStream &operator<<(QDataStream &, const CarrierAddress &);
QDataStream &operator>>(QDataStream &, CarrierAddress &);


#endif
// CARRIERADDRESS_HPP


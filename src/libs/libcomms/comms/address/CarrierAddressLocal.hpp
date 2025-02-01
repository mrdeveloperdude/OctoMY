#ifndef CARRIERADDRESSLOCAL_HPP
#define CARRIERADDRESSLOCAL_HPP

#include <QVariantMap>

#include "CarrierAddress.hpp"

class CarrierAddressLocal: public CarrierAddress
{
private:
	QString mLocalAddress;
	
public:
	explicit CarrierAddressLocal();
	CarrierAddressLocal(const CarrierAddressLocal &other);
	explicit CarrierAddressLocal(const QString &localAddress);
	explicit CarrierAddressLocal(QVariantMap map);
	virtual ~CarrierAddressLocal();

	// Local spesific
public:
	QString address() const;
	void setAddress(const QString &address);
	
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


#endif // CARRIERADDRESSLOCAL_HPP

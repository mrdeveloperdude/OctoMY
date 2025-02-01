#ifndef CARRIERADDRESSBLUETOOTH_HPP
#define CARRIERADDRESSBLUETOOTH_HPP


#include <QBluetoothAddress>
#include <QVariantMap>


#include "CarrierAddress.hpp"

class CarrierAddressBluetooth: public CarrierAddress
{
private:
	QBluetoothAddress mBluetoothAddress;

public:
	explicit CarrierAddressBluetooth();
	CarrierAddressBluetooth(const CarrierAddressBluetooth &other);
	explicit CarrierAddressBluetooth(const QString &localAddress);
	explicit CarrierAddressBluetooth(const quint64 address);
	explicit CarrierAddressBluetooth(QVariantMap map);
	virtual ~CarrierAddressBluetooth();
	
	// Local spesific
public:
	QBluetoothAddress address() const;
	void setAddress(const QBluetoothAddress &address);
	
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



#endif // CARRIERADDRESSBLUETOOTH_HPP

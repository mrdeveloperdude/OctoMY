#ifndef SIMPLEMAPSTORE_HPP
#define SIMPLEMAPSTORE_HPP

#include "uptime/ConfigureHelper.hpp"

#include "SimpleDataStore.hpp"

class SimpleMapStore: public SimpleDataStore
{

private:
	QVariantMap mMap;
	ConfigureHelper mConfigureHelper;

public:
	explicit SimpleMapStore();
	virtual ~SimpleMapStore() override;

public:
	void configure(QString filename);
	void activate(const bool on, std::function<void(bool)> callBack=nullptr);
	bool isActivated() const;

	// SimpleDataStore interface
public:
	bool fromMap(QVariantMap data)  override;
	QVariantMap toMap() override;
	bool fromDefault()  override;

};


#endif
// SIMPLEMAPSTORE_HPP


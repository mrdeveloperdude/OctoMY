#ifndef SIMPLEMAPSTORE_HPP
#define SIMPLEMAPSTORE_HPP

#include "SimpleDataStore.hpp"



class SimpleMapStore: public SimpleDataStore
{

private:
	QVariantMap mMap;

public:
	explicit SimpleMapStore();
	virtual ~SimpleMapStore() Q_DECL_OVERRIDE;

public:
	void configure(QString filename);
	void activate(const bool on, std::function<void(bool)> callBack=nullptr);

	// SimpleDataStore interface
public:
	bool fromMap(QVariantMap data)  Q_DECL_OVERRIDE;
	QVariantMap toMap() Q_DECL_OVERRIDE;
	bool fromDefault()  Q_DECL_OVERRIDE;

};



#endif
// SIMPLEMAPSTORE_HPP


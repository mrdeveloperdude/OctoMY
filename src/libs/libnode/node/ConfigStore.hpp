#ifndef CONFIGSTORE_HPP
#define CONFIGSTORE_HPP

#include "basic/AsyncStore.hpp"

#include <QObject>
#include <QSharedPointer>

class PoseMapping;
/*

  ConfigStore is used to persist the physical configuration for nodes. It may be extended for convenience

  The store supports loading and saving asynchrounusly.

*/



class ConfigStore: public AsyncStore
{
	Q_OBJECT
private:
	QVariantMap mConfig;
public:
	explicit ConfigStore(QString filename="", QObject *parent = nullptr);
	virtual ~ConfigStore();

	// Config selectors
public:
	QVariantMap config() const;
	void setConfig(QVariantMap config);

	void setHookSignals(QObject &ob, bool hook);


	void clear();


	// AsyncStore interface
protected:
	void bootstrapWorkerImpl() Q_DECL_OVERRIDE;
public:
	void load() Q_DECL_OVERRIDE;
	void save() Q_DECL_OVERRIDE;

signals:

	void configStoreReady(bool);

public:

	friend const QDebug &operator<<(QDebug &d, ConfigStore &ks);
};


const QDebug &operator<<(QDebug &d, ConfigStore &ks);


#endif // CONFIGSTORE_HPP

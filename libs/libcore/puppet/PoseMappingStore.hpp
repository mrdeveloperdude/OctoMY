#ifndef POSEMAPPINGSTORE_HPP
#define POSEMAPPINGSTORE_HPP

#include "basic/AsyncStore.hpp"


#include <QObject>
#include <QSharedPointer>

class PoseMapping;
/*

  PoseMappingStore is used to persist the pose mapping for agent.

  The store supports loading and saving of the list asynchrounusly.

*/



class PoseMappingStore: public AsyncStore
{
	Q_OBJECT
private:

	QSharedPointer<PoseMapping> mPoseMapping;
public:
	explicit PoseMappingStore(QString filename="", QObject *parent = nullptr);
	virtual ~PoseMappingStore();

public:
	QSharedPointer<PoseMapping> poseMapping();
	void setPoseMapping(QSharedPointer<PoseMapping> mapping);

	// AsyncStore interface
protected:
	void bootstrapWorkerImpl() Q_DECL_OVERRIDE;
public:
	void load() Q_DECL_OVERRIDE;
	void save() Q_DECL_OVERRIDE;

signals:

	void poseMappingStoreReady(bool);
};

#endif // POSEMAPPINGSTORE_HPP

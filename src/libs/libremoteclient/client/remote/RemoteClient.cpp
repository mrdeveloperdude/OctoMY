#include "RemoteClient.hpp"

#include "RemoteClientWidget.hpp"
#include "comms/couriers/sets/RemoteCourierSet.hpp"


RemoteClient::RemoteClient(QObject *parent)
	: Client(parent)
{
	OC_METHODGATE();
	// Not needed?
	//updateCourierRegistration();
}


RemoteClient::~RemoteClient()
{
	OC_METHODGATE();
}

void RemoteClient::configure(QSharedPointer<Node> node, QSharedPointer<Associate> nodeAssoc)
{
	Client::configure(node, nodeAssoc);
	if(!nodeAssoc.isNull()) {
		QSharedPointer<RemoteClient> sharedThis = qSharedPointerCast<RemoteClient>(sharedFromThis());
		if(!sharedThis.isNull()) {
			mCouriers = QSharedPointer<RemoteCourierSet>::create();
			if(!mCouriers.isNull()) {
				mCouriers->configure(nodeAssoc->id(), sharedThis);
			} else {
				qWarning()<<"ERROR: Could not allocate RemoteCourierSet";
			}
		} else {
			qWarning()<<"ERROR: No shared this";
		}

	} else {
		qWarning()<<"ERROR: No associate";
	}
}

// Client interface


QSharedPointer<CourierSet> RemoteClient::courierSet()
{
	OC_METHODGATE();
	return mCouriers;
}



QSharedPointer<ClientWidget> RemoteClient::widget()
{
	OC_METHODGATE();
	if(mWidget.isNull()) {
		QSharedPointer<RemoteClient> sharedThis = qSharedPointerCast<RemoteClient>(sharedFromThis());
		if(!sharedThis.isNull()) {
			mWidget = QSharedPointer<RemoteClientWidget>::create(sharedThis, nullptr);
			if(mWidget.isNull()) {
				qWarning()<<"ERROR: could not create RemoteClientWidget for RemoteClient";
			}
		} else {
			qWarning()<<"ERROR: sharedFromThis was null for RemoteClient";
		}
	}
	return mWidget;
}


#include "HubClient.hpp"

#include "HubClientWidget.hpp"

HubClient::HubClient(QObject *parent)
	: Client(parent)
{
	OC_METHODGATE();
}


HubClient::~HubClient()
{
	OC_METHODGATE();
}


void HubClient::configure(QSharedPointer<Node> node, QSharedPointer<Associate> nodeAssoc)
{
	Client::configure(node, nodeAssoc);
	if(!nodeAssoc.isNull()) {
		QSharedPointer<HubClient> sharedThis = qSharedPointerCast<HubClient>(sharedFromThis());
		if(!sharedThis.isNull()) {
			mCouriers = QSharedPointer<HubClientCourierSet>::create();
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



QSharedPointer<CourierSet> HubClient::courierSet()
{
	OC_METHODGATE();
	return mCouriers;
}



QSharedPointer<ClientWidget> HubClient::widget()
{
	OC_METHODGATE();
	if(mWidget.isNull()) {
		QSharedPointer<HubClient> sharedThis = qSharedPointerCast<HubClient>(sharedFromThis());
		if(!sharedThis.isNull()) {
			mWidget = QSharedPointer<HubClientWidget>::create(sharedThis, nullptr);
			if(mWidget.isNull()) {
				qWarning()<<"ERROR: could not create RemoteClientWidget for HubClient";
			}
		} else {
			qWarning()<<"ERROR: sharedFromThis was null for HubClient";
		}
	}
	return mWidget;
}


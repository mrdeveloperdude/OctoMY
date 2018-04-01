#include "HubClient.hpp"

#include "HubClientWidget.hpp"

HubClient::HubClient(QSharedPointer<Node> controller, QSharedPointer<Associate> nodeAssoc, QObject *parent)
	: Client(controller, nodeAssoc, parent)
//	, mCouriers(nodeAssoc->id(), *this)
	, mWidget(nullptr)
{
	OC_METHODGATE();
	updateCourierRegistration();
}

/////////////////////////////////////////////////////
/// interface
///


CourierSet &HubClient::courierSet()
{
	OC_METHODGATE();
	return mCouriers;
}



ClientWidget *HubClient::widget()
{
	OC_METHODGATE();
	if(nullptr == mWidget) {
		QSharedPointer<HubClient> sharedThis = qSharedPointerCast<HubClient>(sharedFromThis());
		if(!sharedThis.isNull()) {
			mWidget=OC_NEW HubClientWidget(sharedThis, nullptr);
			if(nullptr == mWidget) {
				qWarning()<<"ERROR: could not create RemoteClientWidget for HubClient";
			}
		} else {
			qWarning()<<"ERROR: sharedFromThis was null for HubClient";
		}
	}
	return mWidget;
}


#include "RemoteClient.hpp"

#include "RemoteClientWidget.hpp"

RemoteClient::RemoteClient(QSharedPointer<Node> controller, QSharedPointer<Associate> nodeAssoc, QObject *parent)
	: Client(controller, nodeAssoc, parent)
	, mCouriers(nodeAssoc->id(), *this)
	, mWidget(nullptr)
{
	OC_METHODGATE();
	// Not needed?
	//updateCourierRegistration();
}


RemoteClient::~RemoteClient()
{

}

/////////////////////////////////////////////////////
/// interface
///


CourierSet &RemoteClient::courierSet()
{
	OC_METHODGATE();
	return mCouriers;
}



ClientWidget *RemoteClient::widget()
{
	OC_METHODGATE();
	if(nullptr == mWidget) {
		QSharedPointer<RemoteClient> sharedThis = qSharedPointerCast<RemoteClient>(sharedFromThis());
		if(!sharedThis.isNull()) {
			mWidget=OC_NEW RemoteClientWidget(sharedThis, nullptr);
			if(nullptr == mWidget) {
				qWarning()<<"ERROR: could not create RemoteClientWidget for RemoteClient";
			}
		} else {
			qWarning()<<"ERROR: sharedFromThis was null for RemoteClient";
		}
	}
	return mWidget;
}


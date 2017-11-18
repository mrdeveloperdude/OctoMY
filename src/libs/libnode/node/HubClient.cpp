#include "HubClient.hpp"

#include "HubClientWidget.hpp"

HubClient::HubClient(QSharedPointer<Node> controller, QSharedPointer<Associate> nodeAssoc, QObject *parent)
	: Client(controller, nodeAssoc, parent)
//	, mCouriers(nodeAssoc->id(), *this)
	, mWidget(nullptr)
	, mThis(this)
{
	OC_METHODGATE();
}

/////////////////////////////////////////////////////
/// interface
///


CourierSet &HubClient::courierSet()
{
	OC_METHODGATE();
	return mCouriers;
}


QWidget *HubClient::widget()
{
	OC_METHODGATE();
	if(nullptr == mWidget) {
		mWidget=OC_NEW HubClientWidget(mThis, nullptr);
	}
	return mWidget;
}



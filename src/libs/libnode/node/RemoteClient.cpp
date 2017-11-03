#include "RemoteClient.hpp"

#include "RemoteClientWidget.hpp"

RemoteClient::RemoteClient(QSharedPointer<Node> controller, QSharedPointer<Associate> nodeAssoc, QObject *parent)
	: Client(controller, nodeAssoc, parent)
	, mCouriers(nodeAssoc->id(), *this)
	, mWidget(nullptr)
	, mThis(this)
{
	OC_METHODGATE();
}

/////////////////////////////////////////////////////
/// interface
///


CourierSet &RemoteClient::courierSet()
{
	OC_METHODGATE();
	return mCouriers;
}



QWidget *RemoteClient::widget()
{
	OC_METHODGATE();
	if(nullptr == mWidget) {
		mWidget=OC_NEW RemoteClientWidget(mThis, nullptr);
	}
	return mWidget;
}



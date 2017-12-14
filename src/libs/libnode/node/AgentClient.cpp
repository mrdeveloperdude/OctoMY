#include "AgentClient.hpp"

#include "AgentClientWidget.hpp"

AgentClient::AgentClient(QSharedPointer<Node> controller, QSharedPointer<Associate> nodeAssoc, QObject *parent)
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


CourierSet &AgentClient::courierSet()
{
	OC_METHODGATE();
	return mCouriers;
}



QWidget *AgentClient::widget()
{
	OC_METHODGATE();
	if(nullptr == mWidget) {
		mWidget=OC_NEW AgentClientWidget(QEnableSharedFromThis<AgentClient>::sharedFromThis(), nullptr);
	}
	return mWidget;
}



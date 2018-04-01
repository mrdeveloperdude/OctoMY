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



ClientWidget *AgentClient::widget()
{
	OC_METHODGATE();
	if(nullptr == mWidget) {
		QSharedPointer<AgentClient> sharedThis = qSharedPointerCast<AgentClient>(sharedFromThis());
		if(!sharedThis.isNull()) {
			mWidget=OC_NEW AgentClientWidget(sharedThis, nullptr);
			if(nullptr == mWidget) {
				qWarning()<<"ERROR: could not create AgentClientWidget for AgentClient";
			}
		} else {
			qWarning()<<"ERROR: sharedFromThis was null for AgentClient";
		}
	}
	return mWidget;
}



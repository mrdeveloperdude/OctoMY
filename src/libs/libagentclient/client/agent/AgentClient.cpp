#include "AgentClient.hpp"

#include "AgentClientWidget.hpp"

AgentClient::AgentClient(QObject *parent)
	: Client(parent)
{
	OC_METHODGATE();
}

AgentClient::~AgentClient()
{
	OC_METHODGATE();
}

void AgentClient::configure(QSharedPointer<Node> node, QSharedPointer<Associate> nodeAssoc)
{
	Client::configure(node, nodeAssoc);
	if(!nodeAssoc.isNull()) {
		QSharedPointer<AgentClient> sharedThis = qSharedPointerCast<AgentClient>(sharedFromThis());
		if(!sharedThis.isNull()) {
			mCouriers=QSharedPointer<AgentClientCourierSet>(OC_NEW AgentClientCourierSet());
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



QSharedPointer<CourierSet> AgentClient::courierSet()
{
	OC_METHODGATE();
	return mCouriers;
}



QSharedPointer<ClientWidget> AgentClient::widget()
{
	OC_METHODGATE();
	if(mWidget.isNull()) {
		QSharedPointer<AgentClient> sharedThis = qSharedPointerCast<AgentClient>(sharedFromThis());
		if(!sharedThis.isNull()) {
			mWidget=QSharedPointer<AgentClientWidget>(OC_NEW AgentClientWidget(sharedThis, nullptr));
			if(mWidget.isNull()) {
				qWarning()<<"ERROR: could not create AgentClientWidget for AgentClient";
			}
		} else {
			qWarning()<<"ERROR: sharedFromThis was null for AgentClient";
		}
	}
	return mWidget;
}



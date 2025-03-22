#include "AgentSelectActivity.hpp"
#include "ui_AgentSelectActivity.h"

#include "AgentItemDelegate.hpp"
#include "node/Node.hpp"
#include "remote/AgentListModel.hpp"
#include "uptime/MethodGate.hpp"

AgentSelectActivity::AgentSelectActivity(QWidget *parent)
	: Activity(parent)
	, ui(new Ui::AgentSelectActivity)
	, mConfigureHelper("AgentSelectActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


AgentSelectActivity::~AgentSelectActivity()
{
	OC_METHODGATE();
	delete ui;
}


void AgentSelectActivity::configure(QSharedPointer<Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = node;
		if(!mNode.isNull()){
			if(nullptr == ui->listViewAgents->model()) {
				if(nullptr == mList) {
					mList = OC_NEW AgentListModel(mNode->addressBook());
				}
				ui->listViewAgents->setModel(mList);
				if(nullptr == mDelegate) {
					mDelegate = OC_NEW AgentItemDelegate(this);
				}
				ui->listViewAgents->setItemDelegate(mDelegate);
				if(nullptr != mDelegate) {
					if(!connect(mDelegate, &AgentItemDelegate::startEdit, [this](const QString id) {
							qDebug() << "EDIT STARTED FOR " << id;
							push("PairingTrustActivity", QStringList() << id);
						})) {
						qWarning() << "ERROR: Could not connect edit " << mDelegate->objectName();
					}
					if(!connect(mDelegate, &AgentItemDelegate::startControl, [this](const QString id) {
							qDebug() << "CONTROL STARTED FOR " << id;
							push("RemoteController", QStringList()<<id);
						})) {
						qWarning() << "ERROR: Could not connect control " << mDelegate->objectName();
					}
				}
				else{
					qWarning() << "ERROR: No delegate";
				}
			}
		}
		updateList();
	}
}


void AgentSelectActivity::updateList(){
	if(mList != nullptr){
		mList->invalidate();
	}
}


void AgentSelectActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	qDebug() << "agent select pop args" << returnActivity << returnArguments;
	if(mConfigureHelper.isConfiguredAsExpected()) {
	}
}


void AgentSelectActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	qDebug() << "agent select push args" << arguments;
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(arguments.size() == 1){
			auto id = arguments[0];
		}
	}
}


void AgentSelectActivity::startPairing(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		push("PairingActivity", QStringList());
	}
}


void AgentSelectActivity::control(){
	if(mConfigureHelper.isConfiguredAsExpected()) {
		push("RemoteController");
	}
}

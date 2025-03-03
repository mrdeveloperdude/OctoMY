/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "FaceActivity.hpp"
#include "log/LogModel.hpp"
#include "ui_FaceActivity.h"

#include "address/Associate.hpp"
#include "agent/Agent.hpp"
#include "agent/AgentControls.hpp"
#include "app/Constants.hpp"
#include "comms/CommsSession.hpp"
#include "comms/couriers/sets/AgentCourierSet.hpp"

#include "utility/ui/Ui.hpp"


#include <QScrollBar>


FaceActivity::FaceActivity(QWidget *parent)
	: Activity(parent)
	, ui(OC_NEW Ui::FaceActivity)
	, mConfigureHelper("FaceActivity", true, false, true, true, false)

{
	OC_METHODGATE();
	ui->setupUi(this);
	connect(ui->widgetConnectionStatus, &ConnectionStatusWidget::openConnectionSettings, this, [this](){
		push("ConnectionActivity");
	});
}


FaceActivity::~FaceActivity()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


QSharedPointer<Agent> FaceActivity::agent()
{
	OC_METHODGATE();
	return mAgent;
}


QSharedPointer<Settings> FaceActivity::settings()
{
	OC_METHODGATE();
	if(!mAgent.isNull()) {
		return mAgent->settings();
	}
	return nullptr;
}


void FaceActivity::updateEyes()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		auto doUpdate = false;
		if(!mAgent.isNull()) {
			const auto ass = mAgent->nodeIdentity();
			if(!ass.isNull()) {
				const PortableID pid = ass->toPortableID();
				if(pid.id() != mLastPID.id()) {
					mLastPID = pid;
					doUpdate = true;
				}
			} else {
				qWarning() << "ERROR: no associate while updating eye color of face widget";
			}
		} else {
			qWarning() <<"ERROR: no agent while updating eye color of face widget";
		}
		if(doUpdate) {
			if(!mLastPID.id().isEmpty()) {
				ui->widgetEyes->setPortableID(mLastPID);
			}
			else{
				qWarning() <<"ERROR: id was empty for eyes";
			}
		}
	}
}


void FaceActivity::updateVisibility()
{
	OC_METHODGATE();
	auto s = settings();
	if(!s.isNull()) {
		ui->widgetEyes->setVisible(s->getCustomSettingBool(Constants::AGENT_FACE_EYES_SHOW, true));
		ui->logScroll->setVisible(s->getCustomSettingBool(Constants::AGENT_FACE_LOG_SHOW, true));
		ui->widgetRealtimeValues->setVisible(s->getCustomSettingBool(Constants::AGENT_FACE_STATS_SHOW, true));
		//ui->widgetConnect->setVisible(s->getCustomSettingBool(Constants::AGENT_FACE_ONLINE_BUTTON_SHOW, true));
	}
}


void FaceActivity::appendLog(const QString& text)
{
	OC_METHODGATE();
	
	if(!mAgent.isNull()){
		auto logStorage = mAgent->logStorage();
		if(logStorage){
			logStorage->appendLog(text);
		}
	}
}


void FaceActivity::configure(QSharedPointer<Agent> a)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		mAgent = a;
		if(!mAgent.isNull()){
			mAgent->setHookCommsSignals(*this, true);
			mAgent->setHookColorSignals(*this, true);
			ui->logScroll->configure(mAgent);
		}
		ui->widgetRealtimeValues->setAgent(mAgent);
		auto s = settings();
		if(!s.isNull()) {
			if(s->hasCustomSetting(Constants::AGENT_FACE_SPLITTER)) {
				//qDebug()<<"FOUND SETTING FOR "<<Constants::AGENT_FACE_SPLITTER;
				ui->splitter->restoreState(s->getCustomSettingByteArray(Constants::AGENT_FACE_SPLITTER));
			} else {
				//qDebug()<<"DEFAULT SETTING FOR "<<Constants::AGENT_FACE_SPLITTER_MIDDLE_STATE;
				QList<int> sz;
				const auto ct = ui->splitter->count();
				const auto s = (ui->splitter->height() - ui->splitter->handleWidth() * ct)/ct;
				for(int i=0;i<ct;++i){
					sz.append(s);
				}
				ui->splitter->setSizes(sz);
			}
		}
		updateEyes();
		updateVisibility();
		ui->widgetConnectionStatus->configure(a);
	}
}


void FaceActivity::onSyncParameterChanged(ISyncParameter *sp)
{
	OC_METHODGATE();
	Q_UNUSED(sp);
	//qDebug()<<"Agent ASC changed: "<<sp->toString();
//TODO: THIS IS ALL WRONG
	/*
	SyncParameter<Pose> *targetPoseParameter=qobject_cast< SyncParameter<Pose> >(sp);
	if(nullptr!=targetPoseParameter) {
		Pose targetPose=targetPoseParameter->bestValue(true);
		//qDebug()<<"TARGET POSE: "<<targetPose.toString();
		ui->widgetPose->poseChanged(targetPose);
		qDebug()<<"ARNOLD!";
	} else {
		qWarning()<<"ERROR: sp was nullptr";
	}
	*/
}



void FaceActivity::onSplitterMoved(int, int)
{
	OC_METHODGATE();
	auto s = settings();
	if(!s.isNull()) {
		s->setCustomSettingByteArray(Constants::AGENT_FACE_SPLITTER, ui->splitter->saveState());
	}
}


// An error occurred in comms
void FaceActivity::onCommsError(QString message){
	OC_METHODGATE();
	appendLog("FACE-COMMS: " + message);
}

// A new comms session was added
void FaceActivity::onCommsClientAdded(CommsSession *c){
	OC_METHODGATE();
	auto address = c->address();
	// TODO: With the new logging system ,this localized logging should no longer be necessary
	appendLog("FACE-COMMS: session added" + (address.isNull()?"NULL":address->toString()) );
}

// The connection state changed for comms channel
void FaceActivity::onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection){
	OC_METHODGATE();
	appendLog(QString("FACE-COMMS: connected=%1, wants connection=%2 ").arg(isConnected).arg(needsConnection) );
}


void FaceActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	qDebug() << "face activity push args" << arguments;
	updateEyes();
	updateVisibility();
}


void FaceActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	qDebug() << "face activity pop args" << returnActivity << returnArguments;
}

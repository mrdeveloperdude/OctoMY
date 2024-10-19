/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "FaceWidget.hpp"
#include "ui_FaceWidget.h"

#include "address/Associate.hpp"
#include "agent/Agent.hpp"
#include "agent/AgentControls.hpp"
#include "app/Constants.hpp"
#include "comms/CommsSession.hpp"
#include "comms/couriers/sets/AgentCourierSet.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/SharedPointerWrapper.hpp"
#include "utility/random/Random.hpp"
#include "utility/ui/Ui.hpp"


#include <QScrollBar>


FaceWidget::FaceWidget(QWidget *parent)
	: Activity(parent)
	, ui(OC_NEW Ui::FaceWidget)
	, mAgent(nullptr)
	, mConfigureHelper("FaceWidget", true, false, true, true, false)

{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->tryToggleConnect->configure("Connect","Connecting...","Connected", "Disconnecting...", Constants::AGENT_CONNECT_BUTTON_COLOR, Constants::AGENT_DISCONNECT_COLOR);
	appendLog("FACE-COMMS: CTOR");
}

FaceWidget::~FaceWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


QSharedPointer<Agent> FaceWidget::agent()
{
	OC_METHODGATE();
	return mAgent;
}

QSharedPointer<Settings> FaceWidget::settings()
{
	OC_METHODGATE();
	if(!mAgent.isNull()) {
		return mAgent->settings();
	}
	return nullptr;
}

void FaceWidget::updateEyeColor()
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
				} else {
					qWarning() << "ERROR: no change in ID while updating eye color of face widget: '" << pid.id() << "'";
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
		}
	}
}


void FaceWidget::updateVisibility()
{
	OC_METHODGATE();
	auto s = settings();
	if(!s.isNull()) {
		ui->widgetEyes->setVisible(s->getCustomSettingBool(Constants::AGENT_FACE_EYES_SHOW, true));
		ui->logScroll->setVisible(s->getCustomSettingBool(Constants::AGENT_FACE_LOG_SHOW, true));
		ui->widgetRealtimeValues->setVisible(s->getCustomSettingBool(Constants::AGENT_FACE_STATS_SHOW, true));
		ui->widgetConnect->setVisible(s->getCustomSettingBool(Constants::AGENT_FACE_ONLINE_BUTTON_SHOW, true));
	}
}


void FaceWidget::appendLog(const QString& text)
{
	OC_METHODGATE();
	ui->logScroll->appendLog(text);
}



void FaceWidget::configure(QSharedPointer<Agent> a)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
			
		mAgent = a;
			
		if(!mAgent.isNull()){
			mAgent->setHookCommsSignals(*this, true);
			mAgent->setHookColorSignals(*this, true);
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
		updateEyeColor();
		updateVisibility();
	}
}
	

// Hook face signals. Might go away since we handle most stuff internaly
void FaceWidget::setHookSignals(QObject &ob, bool hook)
{
	OC_METHODGATE();
	if(hook) {
		if(!connect(this,SIGNAL(connectionStateChanged(TryToggleState, TryToggleState)),&ob,SLOT(onConnectionStateChanged(TryToggleState, TryToggleState)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
		if(!connect(this,SIGNAL(colorChanged(QColor)),&ob,SLOT(onColorChanged(QColor)),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
		if(!connect(this,SIGNAL(panic()),&ob,SLOT(onPanic()),OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect "<<ob.objectName();
		}
	} else {
		if(!disconnect(this,SIGNAL(connectionStateChanged(TryToggleState, TryToggleState)),&ob,SLOT(onConnectionStateChanged(TryToggleState, TryToggleState)))) {
			qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
		}
		if(!disconnect(this,SIGNAL(colorChanged(QColor)),&ob,SLOT(onColorChanged(QColor)))) {
			qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
		}
		if(!disconnect(this,SIGNAL(panic()),&ob,SLOT(onPanic()))) {
			qWarning()<<"ERROR: Could not disconnect "<<ob.objectName();
		}
	}
}




void FaceWidget::onSyncParameterChanged(ISyncParameter *sp)
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

void FaceWidget::onColorClicked()
{
	OC_METHODGATE();
	QPalette p = ui->pushButtonNewColor->palette();
	QColor col;
	col.setHslF(utility::random::frand(), 1.0f, 0.5f);
	p.setColor(QPalette::Button, col);
	ui->pushButtonNewColor->setPalette(p);
	appendLog(QString("New color: %1").arg(col.name()));
	emit colorChanged(col);
}

void FaceWidget::onPanicToggled(bool panic)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		if(!mAgent.isNull()) {
			mAgent->setPanic(panic);
			if(panic) {
				QString str="P A N I C !";
				qWarning()<<str;
				appendLog(str);
			} else {
				QString str="Panic averted";
				qWarning()<<str;
				appendLog(str);
			}
		}
		else{
			QString str="Panic could not be propegated :-|";
			qWarning()<<str;
			appendLog(str);
		}
	}
}



void FaceWidget::onSplitterMoved(int, int)
{
	OC_METHODGATE();
	auto s = settings();
	if(!s.isNull()) {
		s->setCustomSettingByteArray(Constants::AGENT_FACE_SPLITTER, ui->splitter->saveState());
	}
}


// An error occurred in comms
void FaceWidget::onCommsError(QString message){
	appendLog("FACE-COMMS: " + message);
}

// A new comms session was added
void FaceWidget::onCommsClientAdded(CommsSession *c){
	appendLog("FACE-COMMS: session added" + c->address() );
}

// The connection state changed for comms channel
void FaceWidget::onCommsConnectionStatusChanged(const bool isConnected, const bool needsConnection){
	appendLog(QString("FACE-COMMS: connected=%1, wants connection=%2 ").arg(isConnected).arg(needsConnection) );
	const auto tts = createTryToggleState(isConnected, needsConnection);
	ui->tryToggleConnect->setState(tts);
}


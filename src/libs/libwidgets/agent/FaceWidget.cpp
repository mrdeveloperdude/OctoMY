/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "FaceWidget.hpp"
#include "ui_FaceWidget.h"

#include "uptime/ConnectionType.hpp"
#include "utility/ui/Ui.hpp"
#include "utility/random/Random.hpp"
#include "agent/Agent.hpp"
#include "identity/Identicon.hpp"
#include "comms/couriers/AgentStateCourier.hpp"
#include "app/Constants.hpp"
#include "agent/AgentControls.hpp"
#include "comms/couriers/sets/AgentCourierSet.hpp"
#include "address/Associate.hpp"
#include "uptime/SharedPointerWrapper.hpp"


#include <QScrollBar>


FaceWidget::FaceWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::FaceWidget)
	, mAgent(nullptr)
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->tryToggleConnect->configure("Connect","Connecting...","Connected", "Disconnecting...", Constants::AGENT_CONNECT_BUTTON_COLOR, Constants::AGENT_DISCONNECT_COLOR);
	if(!connect(ui->tryToggleConnect, SIGNAL(stateChanged(const TryToggleState, const TryToggleState)), this, SIGNAL(connectionStateChanged(const TryToggleState, const TryToggleState)), OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not forward connectionStateChanged signal";
	}
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


void FaceWidget::updateEyeColor()
{
	OC_METHODGATE();
	bool doUpdate=false;
	if(!mAgent.isNull()) {
		QSharedPointer<Associate> ass=mAgent->nodeIdentity();
		if(!ass.isNull()) {
			const PortableID pid=ass->toPortableID();
			if(pid.id()!=mLastPID.id()) {
				mLastPID=pid;
				doUpdate=true;
			} else {
				qWarning()<<"ERROR: no change in ID while updating eye color of face widget: '"<<pid.id()<<"'";
			}
		} else {
			qWarning()<<"ERROR: no associate while updating eye color of face widget";
		}
	} else {
		qWarning()<<"ERROR: no agent while updating eye color of face widget";
	}
	if(doUpdate) {
		if(!mLastPID.id().isEmpty()) {
			ui->widgetEyes->setPortableID(mLastPID);
		}
	}
}

void FaceWidget::updateVisibility()
{
	OC_METHODGATE();
	if(!mAgent.isNull()) {
		QSharedPointer<Settings> s=mAgent->settings();
		if(!s.isNull()) {
			ui->widgetEyes->setVisible(s->getCustomSettingBool(Constants::AGENT_FACE_EYES_SHOW, true));
			ui->logScroll->setVisible(s->getCustomSettingBool(Constants::AGENT_FACE_LOG_SHOW, true));
			ui->widgetRealtimeValues->setVisible(s->getCustomSettingBool(Constants::AGENT_FACE_STATS_SHOW, true));
			ui->widgetConnect->setVisible(s->getCustomSettingBool(Constants::AGENT_FACE_ONLINE_BUTTON_SHOW, true));
		}
	}
}


void FaceWidget::appendLog(const QString& text)
{
	OC_METHODGATE();
	ui->logScroll->appendLog(text);
}



void FaceWidget::setAgent(QSharedPointer<Agent> a)
{
	OC_METHODGATE();
	mAgent=a;
	ui->widgetRealtimeValues->setAgent(mAgent);

	if(!mAgent.isNull()) {
		QSharedPointer<Settings> s=mAgent->settings();
		if(!s.isNull()) {
			if(s->hasCustomSetting(Constants::AGENT_FACE_SPLITTER_MIDDLE_STATE)) {
				//qDebug()<<"FOUND SETTING FOR "<<Constants::AGENT_FACE_SPLITTER_MIDDLE_STATE;
				ui->splitterMiddle->restoreState(s->getCustomSettingByteArray(Constants::AGENT_FACE_SPLITTER_MIDDLE_STATE));
			} else {
				//qDebug()<<"DEFAULT SETTING FOR "<<Constants::AGENT_FACE_SPLITTER_MIDDLE_STATE;
				utility::ui::moveSplitter(*ui->splitterMiddle, 0.75);
			}
			if(s->hasCustomSetting(Constants::AGENT_FACE_SPLITTER_TOP_STATE)) {
				ui->splitterTop->restoreState(s->getCustomSettingByteArray(Constants::AGENT_FACE_SPLITTER_TOP_STATE));
			} else {
				utility::ui::moveSplitter(*ui->splitterTop, 0.8);
			}
			if(s->hasCustomSetting(Constants::AGENT_FACE_SPLITTER_BOTTOM_STATE)) {
				ui->splitterBottom->restoreState(s->getCustomSettingByteArray(Constants::AGENT_FACE_SPLITTER_BOTTOM_STATE));
			} else {
				utility::ui::moveSplitter(*ui->splitterBottom, 0.5);
			}
		}
	}
	updateEyeColor();
}


void FaceWidget::setConnectionState(const TryToggleState s, const bool doEmit)
{
	OC_METHODGATE();
	//qDebug()<<"FACE TRYSTATE CHANGED TO: "<<s<<" WITH EMIT="<<doEmit;
	ui->tryToggleConnect->setState(s, doEmit);
}


TryToggleState FaceWidget::connectionState() const
{
	OC_METHODGATE();
	return ui->tryToggleConnect->state();
}



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



void FaceWidget::setPanic(bool panic)
{
	OC_METHODGATE();
	ui->pushButtonPanic->setPanic(panic);
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

void FaceWidget::on_pushButtonNewColor_clicked()
{
	OC_METHODGATE();
	QPalette p=ui->pushButtonNewColor->palette();
	QColor col;
	col.setHslF(utility::random::frand(), 1.0f, 0.5f);
	p.setColor(QPalette::Button,col);
	ui->pushButtonNewColor->setPalette(p);
	emit colorChanged(col);
}

void FaceWidget::on_pushButtonPanic_toggled(bool panic)
{
	OC_METHODGATE();
	if(panic) {
		QString str="P A N I C !";
		qWarning()<<str;
		appendLog(str);
	} else {
		QString str="Panic averted";
		qWarning()<<str;
		appendLog(str);
	}

	if(!mAgent.isNull()) {
		mAgent->setPanic(panic);
	}

}



void FaceWidget::on_splitterTop_splitterMoved(int, int)
{
	OC_METHODGATE();
	if(!mAgent.isNull()) {
		QSharedPointer<Settings> s=mAgent->settings();
		if(!s.isNull()) {
			s->setCustomSettingByteArray(Constants::AGENT_FACE_SPLITTER_TOP_STATE, ui->splitterTop->saveState());
		}
	}
}


void FaceWidget::on_splitterBottom_splitterMoved(int pos, int index)
{
	OC_METHODGATE();
	Q_UNUSED(pos);
	Q_UNUSED(index);
	if(!mAgent.isNull()) {
		QSharedPointer<Settings> s=mAgent->settings();
		if(!s.isNull()) {
			s->setCustomSettingByteArray(Constants::AGENT_FACE_SPLITTER_BOTTOM_STATE, ui->splitterBottom->saveState());
		}
	}
}

void FaceWidget::on_splitterMiddle_splitterMoved(int pos, int index)
{
	OC_METHODGATE();
	Q_UNUSED(pos);
	Q_UNUSED(index);
	if(!mAgent.isNull()) {
		QSharedPointer<Settings> s=mAgent->settings();
		if(!s.isNull()) {
			s->setCustomSettingByteArray(Constants::AGENT_FACE_SPLITTER_MIDDLE_STATE, ui->splitterMiddle->saveState());
		}
	}
}

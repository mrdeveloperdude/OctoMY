/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "FaceWidget.hpp"
#include "ui_FaceWidget.h"

#include "basic/Settings.hpp"
#include "agent/Agent.hpp"

#include "widgets/Identicon.hpp"

#include "comms/couriers/AgentStateCourier.hpp"
#include "../libutil/utility/Utility.hpp"

#include "agent/AgentConstants.hpp"
#include <QScrollBar>



FaceWidget::FaceWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::FaceWidget)
	, mAgent(nullptr)
{
	ui->setupUi(this);

	ui->tryToggleConnect->configure("Go Online", "Connecting..", "Online", AgentConstants::AGENT_CONNECT_BUTTON_COLOR);
	if(!connect(ui->tryToggleConnect,SIGNAL(stateChanged(const TryToggleState, const TryToggleState)),this,SIGNAL(connectionStateChanged(const TryToggleState, const TryToggleState)),OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}

	updateEyeColor();


}

FaceWidget::~FaceWidget()
{
	delete ui;
}


Agent *FaceWidget::agent()
{
	return mAgent;
}


void FaceWidget::updateEyeColor()
{
	if(nullptr!=mAgent) {
		QSharedPointer<NodeAssociate> ass=mAgent->nodeIdentity();
		if(nullptr!=ass) {
			const QString id=ass->id();
			if(id!=lastID) {
				lastID=id;
				PortableID pid=mAgent->nodeIdentity()->toPortableID();
				Identicon identicon;
				identicon.setPortableID(pid);
				ui->widgetEyes->setColor(identicon.bodyColorHigh());
			}
		}
	}
}

void FaceWidget::updateVisibility()
{
	if(nullptr!=mAgent) {
		Settings &s=mAgent->settings();
		ui->widgetEyes->setVisible(s.getCustomSettingBool(AgentConstants::AGENT_FACE_EYES_SHOW,true));
		ui->logScroll->setVisible(s.getCustomSettingBool(AgentConstants::AGENT_FACE_LOG_SHOW,true));
		ui->widgetRealtimeValues->setVisible(s.getCustomSettingBool(AgentConstants::AGENT_FACE_STATS_SHOW,true));
		ui->widgetConnect->setVisible(s.getCustomSettingBool(AgentConstants::AGENT_FACE_ONLINE_BUTTON_SHOW,true));
	}
}


void FaceWidget::appendLog(const QString& text)
{
	ui->logScroll->appendLog(text);
}



void FaceWidget::setAgent(Agent *a)
{
	mAgent=a;
	ui->widgetRealtimeValues->setAgent(mAgent);

	if(nullptr!=mAgent) {
		const AgentControls &ctl=mAgent->controls();
		CourierSet*set=ctl.activeControl();
		if(nullptr!=set) {
			AgentStateCourier * asc=set->agentStateCourier();
			if(nullptr!=asc) {
				asc->setHookSignals(*this,true);
			}

		}
		Settings &s=mAgent->settings();
		if(s.hasCustomSetting(AgentConstants::AGENT_FACE_SPLITTER_MIDDLE_STATE)) {
			//qDebug()<<"FOUND SETTING FOR "<<AgentConstants::AGENT_FACE_SPLITTER_MIDDLE_STATE;
			ui->splitterMiddle->restoreState(s.getCustomSettingByteArray(AgentConstants::AGENT_FACE_SPLITTER_MIDDLE_STATE));
		} else {
			//qDebug()<<"DEFAULT SETTING FOR "<<AgentConstants::AGENT_FACE_SPLITTER_MIDDLE_STATE;
			utility::moveSplitter(*ui->splitterMiddle,0.75f);
		}
		if(s.hasCustomSetting(AgentConstants::AGENT_FACE_SPLITTER_TOP_STATE)) {
			ui->splitterTop->restoreState(s.getCustomSettingByteArray(AgentConstants::AGENT_FACE_SPLITTER_TOP_STATE));
		} else {
			utility::moveSplitter(*ui->splitterTop,0.8f);
		}
		if(s.hasCustomSetting(AgentConstants::AGENT_FACE_SPLITTER_BOTTOM_STATE)) {
			ui->splitterBottom->restoreState(s.getCustomSettingByteArray(AgentConstants::AGENT_FACE_SPLITTER_BOTTOM_STATE));
		} else {
			utility::moveSplitter(*ui->splitterBottom,0.5f);
		}
	}
	updateEyeColor();
}


void FaceWidget::setConnectionState(const TryToggleState s, const bool doEmit)
{
	//qDebug()<<"FACE TRYSTATE CHANGED TO: "<<s<<" WITH EMIT="<<doEmit;
	ui->tryToggleConnect->setState(s, doEmit);
}


TryToggleState FaceWidget::connectionState() const
{
	return ui->tryToggleConnect->state();
}



void FaceWidget::hookSignals(QObject &ob)
{
	if(!connect(this,SIGNAL(connectionStateChanged(TryToggleState, TryToggleState)),&ob,SLOT(onConnectionStateChanged(TryToggleState, TryToggleState)),OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect "<<ob.objectName();
	}
	if(!connect(this,SIGNAL(colorChanged(QColor)),&ob,SLOT(onColorChanged(QColor)),OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect "<<ob.objectName();
	}
	if(!connect(this,SIGNAL(panic()),&ob,SLOT(onPanic()),OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect "<<ob.objectName();
	}
}


void FaceWidget::unHookSignals(QObject &ob)
{
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


void FaceWidget::setPanic(bool panic)
{
	ui->pushButtonPanic->setPanic(panic);
}

void FaceWidget::on_pushButtonNewColor_clicked()
{
	QPalette p=ui->pushButtonNewColor->palette();
	QColor col;
	col.setHslF(((qreal)qrand())/((qreal)RAND_MAX),1.0,0.5);
	p.setColor(QPalette::Button,col);
	ui->pushButtonNewColor->setPalette(p);
	emit colorChanged(col);
}

void FaceWidget::on_pushButtonPanic_toggled(bool panic)
{
	if(panic) {
		QString str="P A N I C !";
		qWarning()<<str;
		appendLog(str);
	} else {
		QString str="Panic averted";
		qWarning()<<str;
		appendLog(str);
	}

	if(nullptr!=mAgent) {
		mAgent->setPanic(panic);
	}

}



void FaceWidget::onSyncParameterChanged(ISyncParameter *sp)
{
	OC_METHODGATE();
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

void FaceWidget::on_splitterTop_splitterMoved(int, int)
{
	if(nullptr!=mAgent) {
		Settings &s=mAgent->settings();
		s.setCustomSettingByteArray(AgentConstants::AGENT_FACE_SPLITTER_TOP_STATE, ui->splitterTop->saveState());
	}
}


void FaceWidget::on_splitterBottom_splitterMoved(int pos, int index)
{
	if(nullptr!=mAgent) {
		Settings &s=mAgent->settings();
		s.setCustomSettingByteArray(AgentConstants::AGENT_FACE_SPLITTER_BOTTOM_STATE, ui->splitterBottom->saveState());
	}
}

void FaceWidget::on_splitterMiddle_splitterMoved(int pos, int index)
{
	if(nullptr!=mAgent) {
		Settings &s=mAgent->settings();
		s.setCustomSettingByteArray(AgentConstants::AGENT_FACE_SPLITTER_MIDDLE_STATE, ui->splitterMiddle->saveState());
	}
}

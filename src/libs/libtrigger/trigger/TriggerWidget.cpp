#include "TriggerWidget.hpp"
#include "ui_TriggerWidget.h"

#include "trigger/action/Action.hpp"
#include "trigger/condition/conditions/CompositeCondition.hpp"
#include "trigger/factory/ConditionFactory.hpp"
#include "trigger/factory/ActionFactory.hpp"
#include "trigger/trigger/Trigger.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QMenu>


TriggerWidget::TriggerWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::TriggerWidget)
	, mConditionMenu(OC_NEW QMenu(tr("ConditionMenu"), this))
	, mActionMenu(OC_NEW QMenu(tr("ActionMenu"), this))
	, mConfigureHelper("TriggerWidget", true, false, true)
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->frame->setStyleSheet("#frame{ border-top: 0.1em solid rgba(255,255,255,0.25); border-left: 0.1em  solid rgba(0,0,0,0.25); border-bottom: 0.1em solid rgba(0,0,0,0.25); border-right: 0.1em solid rgba(255,255,255,0.25);}");
	configureConditionsList();
	configureActionsList();
}



void TriggerWidget::configureConditionsList(){
	OC_METHODGATE();
	ui->labelConditions->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
	ui->labelConditions->setOpenExternalLinks(false);

	
	connect(ui->labelConditions, &QLabel::linkActivated, this, [this](const QString &link_raw) {
		qDebug() << "condition link clicked:" << link_raw;
		if(!mTrigger){
			qWarning() << "No trigger";
			return;
			
		}
		const auto parts = link_raw.split(ParameterPhrase::separator);
		QString trigger_id;
		QString condition_id;
		QString link{link_raw};
		if(3 == parts.size()){
			trigger_id = parts[0];
			condition_id = parts[1];
			link = parts[2];
		}
		qDebug() << "trigger_id=" << trigger_id;
		qDebug() << "condition_id=" << condition_id;
		qDebug() << "link_id=" << link;
		if(!trigger_id.isEmpty() && mTrigger->id() != trigger_id){
			qWarning() << "Incorrect trigger ID specified";
			return;
		}
		auto condition = mTrigger->conditionByID(condition_id);
		if(!condition){
			qWarning() << "Incorrect condition ID specified";
			return;
		}
		auto composite = CompositeCondition::composite_name == condition->name()? qSharedPointerCast<CompositeCondition>(condition):nullptr;
		qDebug() << "condition: " << condition->name() << condition->id();
		if("switch-operator" == link){
			if (composite.isNull()) {
				qWarning() << "Not a valid composite";
				return;
			}
			auto op = composite->logicalOperator();
			op = cycle(op);
			composite->setLogicalOperator(op);
			updateConditions();
		}
		else if("add-condition" == link){
			if (composite.isNull()) {
				qWarning() << "Not a valid composite";
				return;
			}
			mCurrentCondition = composite;
			mConditionMenu->exec(QCursor::pos());
		}
		else if("remove-condition" == link){
			mTrigger->removeCondition(condition_id);
			updateConditions();
		}
		else{
			qWarning() << "Unknown link clicked:" << link;
		}
	});
}


void TriggerWidget::configureActionsList(){
	OC_METHODGATE();
	ui->labelActions->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
	ui->labelActions->setOpenExternalLinks(false);
	
	connect(ui->labelActions, &QLabel::linkActivated, this, [this](const QString &link_raw) {
		qDebug() << "action link clicked:" << link_raw;
		if(!mTrigger){
			qWarning() << "No trigger";
			return;
			
		}
		const auto parts = link_raw.split(ParameterPhrase::separator);
		QString trigger_id;
		QString action_id;
		QString link{link_raw};
		if(3 == parts.size()){
			trigger_id = parts[0];
			action_id = parts[1];
			link = parts[2];
		}
		qDebug() << "trigger_id=" << trigger_id;
		qDebug() << "action_id=" << action_id;
		qDebug() << "link_id=" << link;
		if(!trigger_id.isEmpty() && mTrigger->id() != trigger_id){
			qWarning() << "Incorrect trigger ID specified";
			return;
		}
		if(!action_id.isEmpty()){
			auto action = mTrigger->actionByID(action_id);
			if(!action){
				qWarning() << "Incorrect action ID specified";
				return;
			}
		}
		if("add-action" == link){
			if(mTrigger){
				mActionMenu->exec(QCursor::pos());
			}
		}
		else{
			qWarning() << "Unknown link clicked:" << link;
		}
	});
}


QAction* TriggerWidget::addConditionMenuItem(QString title, QString icon, QString toolTip) {
	OC_METHODGATE();
	QAction *action=OC_NEW QAction(title, this);
	if(!(connect(action, &QAction::triggered, [title, this](){
			//qDebug()<<"triggered" << title;
			//this->addCondition(title);
			if(mCurrentCondition.isNull()){
				qWarning() << "Can't add to null condition";
				return;
			}
			mCurrentCondition->addCondition(ConditionFactory::instance().create(title));
			updateConditions();
		}))){
		qWarning() << "Could not connect action method for" << action->text() << "to menu";
	}else{
		action->setStatusTip(toolTip);
		action->setIcon(QIcon(icon));
		//qDebug()<<"adding condition to menu"<<action->text();
		mConditionMenu->addAction(action);
	}
	return action;
}


QAction* TriggerWidget::addActionMenuItem(QString title, QString icon, QString toolTip) {
	OC_METHODGATE();
	QAction *action=OC_NEW QAction(title, this);
	if(!(connect(action, &QAction::triggered, [title, this](){
			//qDebug()<<"triggered" << title;
			this->addAction(title);
		}))){
		qWarning() << "Could not connect action method for" << action->text() << "to menu";
	}else{
		action->setStatusTip(toolTip);
		action->setIcon(QIcon(icon));
		//qDebug()<<"adding action to menu"<<action->text();
		mActionMenu->addAction(action);
	}
	return action;
}


void TriggerWidget::prepareMenus(){
	OC_METHODGATE();
	qDebug()<<"Preparing menu items";
	for(auto conditionName:ConditionFactory::instance().available()){
		//qDebug()<<"Preparing menu item "<< conditionName;
		addConditionMenuItem(conditionName, ":/icons/trigger/condition.svg", "Condition");
	}
	for(auto actionName:ActionFactory::instance().available()){
		//qDebug()<<"Preparing menu item "<< actionName;
		addActionMenuItem(actionName, ":/icons/trigger/action.svg", "Action");
	}
}


void TriggerWidget::updateConditions(){
	if(mConfigureHelper.configure()){
		QString html;
		if(mTrigger) {
			const auto trigger_id = mTrigger->id();
			auto const conditions = mTrigger->conditions();
			if(conditions){
				//qDebug()<< "----------------------------------------------";
				html = conditions->toHTML(trigger_id, mEditMode);
				//qDebug() << "Conditions" << conditions->name();
				//qDebug().noquote().nospace() << html;
			}
		}
		ui->labelConditions->setText(html);
	}
}


void TriggerWidget::updateActions(){
	if(mConfigureHelper.configure()){
		QString html;
		bool active{false};
		if(mTrigger) {
			active = mTrigger->isActive();
			const auto trigger_id = mTrigger->id();
			html +="<table>\n";
			const auto actions{mTrigger->actions()};
			for(const auto &action:actions) {
				if(action) {
					auto child_base_prefix{trigger_id + ParameterPhrase::separator  + action->id() + ParameterPhrase::separator};
					if(mEditMode){
						html += QString(" <tr><td>%1").arg(action->phrase()->filledPhrase(child_base_prefix));
						html += QString(" <a href=\"%1\"><img src=\":/icons/app/add.svg\" height=13/></a>\n").arg(child_base_prefix + "add-action");
						html += QString(" <a href=\"%1\"><img src=\":/icons/app/arrow_up.svg\" height=13/></a>\n").arg(child_base_prefix + "move-up-action");
						html += QString(" <a href=\"%1\"><img src=\":/icons/app/arrow_down.svg\"  height=13/></a>\n").arg(child_base_prefix + "move-down-action");
						html += QString(" <a href=\"%1\"><img src=\":/icons/app/remove.svg\" height=13/></a>\n").arg(child_base_prefix + "remove-action");
					}
					else{
						html += QString(" <tr><td>%1").arg(action->phrase()->filledPhrase());
					}
					html += "</td></tr>\n";
				}
			}
			if(mEditMode && actions.empty()){
				html += QString(" <li><a href=\"%1\"><img src=\":/icons/app/add.svg\" height=13/></a></li>\n").arg("add-action");
			}
			else{
				html += QString(" <li><a href=\"#\"><img src=\":/icons/app/transparent.svg\" height=13 /></a></li>\n");
			}
			html += "</table>\n";
		}
		ui->lightWidgetActive->setLightOn(active);
		ui->labelActions->setText(html);
	}
}


void TriggerWidget::configure(QSharedPointer<Trigger> trigger)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		mTrigger = trigger;
		if(!mTrigger) {
			qDebug() << "No trigger";
			return;
		}
		updateConditions();
		updateActions();
		toggleEnabled(mTrigger->isEnabled());
		prepareMenus();
	}
}


TriggerWidget::~TriggerWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


void TriggerWidget::remove(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		//qDebug()<<"Remove trigger";
		
	}
}


void TriggerWidget::toggleEnabled(bool enabled)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		ui->labelActions->setEnabled(enabled);
		ui->labelActionsTitle->setEnabled(enabled);
		ui->labelConditions->setEnabled(enabled);
		ui->labelConditionsTitle->setEnabled(enabled);
		ui->checkBoxEnabled->setChecked(enabled);
		/*
		ui->pushButtonAddAction->setEnabled(enabled);
		ui->pushButtonAddCondition->setEnabled(enabled);
		ui->pushButtonEdit->setEnabled(enabled);
		ui->pushButtonRemove->setEnabled(enabled);
	*/
		if(nullptr!=mTrigger) {
			mTrigger->setEnabled(enabled);
		}
	}
}

void TriggerWidget::toggleEdit(bool edit){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		mEditMode = edit;
		updateConditions();
		updateActions();
	}
}

void TriggerWidget::addCondition(const QString &name){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		qDebug() << "Add condition" << name;
		if(mTrigger){
			mTrigger->addCondition(ConditionFactory::instance().create(name));
			updateConditions();
		}
	}
}


void TriggerWidget::addAction(const QString &name){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		qDebug() << "Add action" << name;
		if(mTrigger){
			mTrigger->addAction(ActionFactory::instance().create(name));
			updateActions();
		}
	}
}

void TriggerWidget::enterEvent(QEnterEvent *event) {
	OC_METHODGATE();
	Q_UNUSED(event);
	toggleEdit(true);
}

void TriggerWidget::leaveEvent(QEvent *event) {
	OC_METHODGATE();
	Q_UNUSED(event);
	toggleEdit(false);
}

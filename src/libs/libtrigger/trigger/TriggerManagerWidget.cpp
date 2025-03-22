#include "TriggerManagerWidget.hpp"
#include "ui_TriggerManagerWidget.h"

#include "trigger/trigger/Trigger.hpp"
#include "trigger/TriggerWidget.hpp"
#include "trigger/executor/TriggerExecutor.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QSpacerItem>


TriggerManagerWidget::TriggerManagerWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::TriggerManagerWidget)
	, mTriggers(nullptr)
	, mConfigureHelper("TriggerManagerWidget", true, false, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

TriggerManagerWidget::~TriggerManagerWidget()
{
	OC_METHODGATE();
	delete ui;
	ui = nullptr;
}


void TriggerManagerWidget::configure(QSharedPointer<TriggerExecutor> set)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		mTriggers = set;
		for(auto &widget: mWidgets) {
			if(widget) {
				removeWidget(widget);
				widget->deleteLater();
			}
		}
		if(mTriggers){
			auto triggers = mTriggers->triggers();
			for(auto &trigger: triggers) {
				if(trigger) {
					auto widget = QSharedPointer<TriggerWidget>::create(nullptr);
					if(widget) {
						widget->configure(trigger);
						addWidget(widget);
					}
				}
			}
		}
	}
}


void TriggerManagerWidget::addWidget(QSharedPointer<QWidget> widget)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		qDebug()<<"--- ADDING WIDGET TO TRIGGER MANAGER";
		if(!widget){
			qWarning()<<"Trying to add null widget";
			return;
		}
		if(!mWidgets.contains(widget)) {
			mWidgets.push_back(widget);
			ui->widgetCompressedContent->layout()->addWidget(widget.data());
			widget->setParent(ui->widgetCompressedContent);
			QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
			widget->setMinimumSize(0, 0);
			widget->setSizePolicy(sizePolicy);
			widget->updateGeometry();
			widget->adjustSize();
		}
	}
}


void TriggerManagerWidget::removeWidget(QSharedPointer<QWidget> w)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		//qDebug()<<"--- REMOVING WIDGET FROM TRIGGER MANAGER";
		if(mWidgets.contains(w)) {
			mWidgets.removeAll(w);
			ui->widgetCompressedContent->layout()->removeWidget(w.data());
			w->setParent(nullptr);
		}
	}
}


QSharedPointer<TriggerWidget> TriggerManagerWidget::addTriggerWidget(QSharedPointer<Trigger> trigger){
	if(mConfigureHelper.isConfiguredAsExpected()){
		if(trigger) {
			auto triggerWidget = QSharedPointer<TriggerWidget>::create(nullptr);
			if(triggerWidget) {
				triggerWidget->configure(trigger);
				addWidget(qSharedPointerCast<QWidget>(triggerWidget));
				return triggerWidget;
			}
		}
	}
	return nullptr;
}


void TriggerManagerWidget::addTrigger(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		qDebug() << "Add trigger";
		auto trigger = QSharedPointer<Trigger>::create();
		if(trigger){
			if(mTriggers){
				mTriggers->addTrigger(trigger);
				addTriggerWidget(trigger);
			}
		}
	}
}


void TriggerManagerWidget::resetExecution(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		if(mTriggers){
			mTriggers->reset();
		}
	}
}
void TriggerManagerWidget::stepExecution(){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		if(mTriggers){
			mTriggers->step();
		}
	}
}

void TriggerManagerWidget::runExecution(bool run){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		ui->pushButtonStep->setEnabled(!run);
		if(mTriggers){
			mTriggers->run(run);
		}
	}
}


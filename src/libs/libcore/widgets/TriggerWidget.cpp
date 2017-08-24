#include "TriggerWidget.hpp"
#include "ui_TriggerWidget.h"

#include "trigger/Trigger.hpp"
#include "trigger/Condition.hpp"
#include "trigger/Action.hpp"

TriggerWidget::TriggerWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::TriggerWidget)
	, mTrigger(nullptr)
{
	ui->setupUi(this);
	ui->frame->setStyleSheet("#frame{ border-top: 0.1em solid rgba(255,255,255,0.25); border-left: 0.1em  solid rgba(0,0,0,0.25); border-bottom: 0.2em solid rgba(0,0,0,0.25); border-right: 0.2em solid rgba(255,255,255,0.25);}");
}

void TriggerWidget::configure(Trigger *trigger)
{
	mTrigger=trigger;
	if(nullptr!=mTrigger) {
		const bool en=mTrigger->isEnabled();
		ui->checkBoxEnabled->setChecked(en);
		on_checkBoxEnabled_toggled(en);
		QList<Condition *> conditions=trigger->conditions();
		QString conditionHTML=QStringLiteral("<ul>\n");
		QString actionHTML=QStringLiteral("<ul>\n");
		for(Condition *condition:conditions) {
			if(nullptr!=condition) {
				conditionHTML+=QStringLiteral("\t<li>")+condition->toString()+QStringLiteral("</li>\n");
			}
		}
		QList<Action *> actions=trigger->actions();
		for(Action *action:actions) {
			if(nullptr!=action) {
				actionHTML+=QStringLiteral("\t<li>")+action->toString()+QStringLiteral("</li>\n");
			}
		}
		conditionHTML+=QStringLiteral("</ul>\n");
		actionHTML+=QStringLiteral("</ul>\n");
		ui->labelActions->setText(actionHTML);
		ui->labelConditions->setText(conditionHTML);
	}
}

TriggerWidget::~TriggerWidget()
{
	delete ui;
	ui=nullptr;
}

void TriggerWidget::on_checkBoxEnabled_toggled(bool checked)
{
	ui->labelActions->setEnabled(checked);
	ui->labelConditions->setEnabled(checked);
	ui->label_1->setEnabled(checked);
	ui->label2->setEnabled(checked);
	if(nullptr!=mTrigger) {
		mTrigger->setEnabled(checked);
	}
}

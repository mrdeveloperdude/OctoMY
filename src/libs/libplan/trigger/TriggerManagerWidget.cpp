#include "TriggerManagerWidget.hpp"
#include "ui_TriggerManagerWidget.h"

#include "trigger/Trigger.hpp"
#include "trigger/TriggerSet.hpp"
#include "trigger/TriggerWidget.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#ifdef OC_USE_FEATURE_MOTORICS

#include "motorics/WheeledLobeWidget.hpp"
#include "motorics/Lobe.hpp"

#include "motorics/LobeController.hpp"

#endif


#include <QSpacerItem>


TriggerManagerWidget::TriggerManagerWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::TriggerManagerWidget)
	, mTriggers(nullptr)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

TriggerManagerWidget::~TriggerManagerWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


void TriggerManagerWidget::configure(TriggerSet &set)
{
	OC_METHODGATE();
	mTriggers=&set;
	for(QWidget * widget:mWidgets) {
		if(nullptr!=widget) {
			removeWidget(*widget);
			widget->deleteLater();
		}
	}
	for(Trigger *trigger:*mTriggers) {
		if(nullptr!=trigger) {
			TriggerWidget *widget=OC_NEW TriggerWidget(nullptr);
			if(nullptr!=widget) {
				widget->configure(trigger);
				addWidget(*widget);
			}
		}
	}
}

void TriggerManagerWidget::addWidget(QWidget &w)
{
	OC_METHODGATE();
	//qDebug()<<"--- ADDING WIDGET TO TRIGGER MANAGER";
	QWidget *si=&w;
	if(!mWidgets.contains(si)) {
		mWidgets.push_back(si);
		ui->widgetCompressedContent->layout()->addWidget(si);
		si->setParent(ui->widgetCompressedContent);
		QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
		si->setMinimumSize(0,0);
		si->setSizePolicy(sizePolicy);
		si->updateGeometry();
		si->adjustSize();
	}
}

void TriggerManagerWidget::removeWidget(QWidget &w)
{
	OC_METHODGATE();
	//qDebug()<<"--- REMOVING WIDGET FROM TRIGGER MANAGER";
	if(mWidgets.contains(&w)) {
		mWidgets.removeAll(&w);
		ui->widgetCompressedContent->layout()->removeWidget(&w);
		QWidget *si=&w;
		if (nullptr != si)  {
			// Whatever
		}
		w.setParent(nullptr);
	}
}



#include "trigger/Action.hpp"
#include "trigger/Condition.hpp"
#include "trigger/Trigger.hpp"
#include "trigger/TriggerSet.hpp"


TriggerSet *mockTriggers(){
	TriggerSet *ts=OC_NEW TriggerSet;
	Trigger *t1=OC_NEW Trigger("Trigger #1");
	Trigger *t2=OC_NEW Trigger("Trigger #2");
	Trigger *t3=OC_NEW Trigger("Trigger #3");
	t1->addCondition(*OC_NEW Condition("Condition #1", "Formula #1"));
	t1->addCondition(*OC_NEW Condition("Condition #2", "Formula #2"));
	t1->addAction(*OC_NEW Action("Action #1"));
	t1->addAction(*OC_NEW Action("Action #2"));
	
	t2->addCondition(*OC_NEW Condition("Condition #3", "Formula #3"));
	t2->addAction(*OC_NEW Action("Action #3"));
	
	t3->addCondition(*OC_NEW Condition("Condition #4", "Formula #4"));
	t3->addAction(*OC_NEW Action("Action #4"));
	
	*ts<<t1<<t2<<t3;
	
	for(int i=0; i<10; ++i) {
		Trigger *t=OC_NEW Trigger("Trigger #5");
		t->addCondition(*OC_NEW Condition("Condition #5", "Formula #5"));
		t->addAction(*OC_NEW Action("Action #5"));
		*ts<<t;
	}
	return ts;
}

#include "TriggerManagerWidget.hpp"
#include "ui_TriggerManagerWidget.h"

#include "trigger/Trigger.hpp"
#include "trigger/TriggerSet.hpp"
#include "trigger/TriggerWidget.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#ifdef OC_USE_FEATURE_MOTORICS

#include "motorics/WheeledMtlobeWidget.hpp"
#include "motorics/Mtlobe.hpp"

#include "motorics/MtlobeController.hpp"

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

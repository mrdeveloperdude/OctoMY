#include "TriggerManagerWidget.hpp"
#include "ui_TriggerManagerWidget.h"

#include "utility/Standard.hpp"

#include "mtlobe/WheeledMtlobeWidget.hpp"
#include "mtlobe/Mtlobe.hpp"

#include "mtlobe/MtlobeController.hpp"

#include "trigger/TriggerSet.hpp"
#include "widgets/TriggerWidget.hpp"

#include <QSpacerItem>


TriggerManagerWidget::TriggerManagerWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::TriggerManagerWidget)
	, mTriggers(nullptr)
{
	ui->setupUi(this);
}

TriggerManagerWidget::~TriggerManagerWidget()
{
	delete ui;
	ui=nullptr;
}


void TriggerManagerWidget::configure(TriggerSet &set)
{
	mTriggers=&set;
	for(QWidget * widget:mWidgets) {
		if(nullptr!=widget) {
			removeWidget(*widget);
			widget->deleteLater();
		}
	}
	for(Trigger *trigger:*mTriggers) {
		if(nullptr!=trigger) {
			TriggerWidget *widget=new TriggerWidget(nullptr);
			if(nullptr!=widget) {
				widget->configure(trigger);
				addWidget(*widget);
			}
		}
	}
}

void TriggerManagerWidget::addWidget(QWidget &w)
{
	qDebug()<<"--- ADDING WIDGET TO TRIGGER MANAGER";
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
	qDebug()<<"--- REMOVING WIDGET FROM TRIGGER MANAGER";
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

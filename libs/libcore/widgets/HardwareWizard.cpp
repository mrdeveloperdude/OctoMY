#include "HardwareWizard.hpp"
#include "ui_HardwareWizard.h"

#include "HardwareTemplate.hpp"
#include "models/SerialDeviceListModel.hpp"
#include "models/HardwareTemplateModel.hpp"

#include <QDebug>


HardwareWizard::HardwareWizard(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::HardwareWizard)
	, mSerialDevicesModel(new SerialDeviceListModel(this))
	, mHardwareTemplateModel(new HardwareTemplateModel(this))
	, mSelectedTempalte(nullptr)
{
	ui->setupUi(this);
	//qDebug()<<"Setting model";
	ui->listViewSerialInterface->setModel(mSerialDevicesModel);
	ui->listViewTemplate->setModel(mHardwareTemplateModel);
	reset();
}

HardwareWizard::~HardwareWizard()
{
	delete ui;
}


template<typename T>
static long mod(T a, T b)
{
	return (a%b+b)%b;
}

void HardwareWizard::reset()
{
	//We skip the template screen for now, because frankly it is not useful until this menu starts overflowing with alternatives.
	//moveTo(0);
	moveTo(0);
}


static void selectFirstIfNoneSelected(QListView *list)
{
	list->setFocus();
	if(!list->selectionModel()->hasSelection()) {
		list->selectionModel()->setCurrentIndex(list->model()->index(0,0),QItemSelectionModel::Select);
	}
}

void HardwareWizard::moveTo(int next)
{
	switch(next) {
	//Template
	case(0): {
		selectFirstIfNoneSelected(ui->listViewTemplate);
	}
	break;
	// Interface
	case(1): {

		selectFirstIfNoneSelected(ui->listWidgetInterfaceType);
	}
	break;
	// Serial device
	case(2): {
		selectFirstIfNoneSelected(ui->listViewSerialInterface);
	}
	break;
	// Protocol
	case(3): {
		selectFirstIfNoneSelected(ui->listWidgetProtocol);
	}
	break;


	// Serial settings
	case(4): {

	}
	break;


	// Pose
	case(5): {
		//ui->widgetPoseMapping
	}
	break;

	case(6): {
		emit done();
		//selectFirstIfNoneSelected(ui->listWidgetProtocol);
	}
	break;
	}
	ui->stackedWidget->setCurrentIndex( next );
}


void HardwareWizard::save()
{
	const int cur=ui->stackedWidget->currentIndex();
	switch(cur) {
	//Template
	case(0): {
		const int curTemplate=ui->listViewTemplate->currentIndex().row();
		qDebug()<<"Found template: "<<curTemplate;
		QVector<HardwareTemplate *> templates=HardwareTemplate::templates();
		if(curTemplate<0 || curTemplate>=templates.size()) {
			qWarning()<<"ERROR: Could not find selected template";
			return;
		}
		mSelectedTempalte=templates[curTemplate];
		loadFromTemplate();
	}
	break;
	//Interface
	case(1): {

	}
	break;
	//Serial
	case(2): {

	}
	break;
	//Protocol
	case(3): {
	}
	break;
	}
}

void HardwareWizard::loadFromTemplate()
{
	qWarning()<<"TODO: Implement this";
	if(nullptr!=mSelectedTempalte) {
		mPoseMapping=mSelectedTempalte->poseMapping();
		ui->widgetPoseMapping->configure(mPoseMapping);
	}

}

void HardwareWizard::on_pushButtonOnward_clicked()
{
	save();
	const int cur=ui->stackedWidget->currentIndex();
	const int next=mod(cur+1, ui->stackedWidget->count());
	moveTo(next);
}

void HardwareWizard::on_pushButtonBack_clicked()
{
	const int next=mod(ui->stackedWidget->currentIndex()-1, ui->stackedWidget->count());
	moveTo(next);
}

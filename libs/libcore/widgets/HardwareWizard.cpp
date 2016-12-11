#include "HardwareWizard.hpp"
#include "ui_HardwareWizard.h"

#include "SerialDeviceListModel.hpp"

#include <QDebug>

HardwareWizard::HardwareWizard(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::HardwareWizard)
	, mSerialDevices(new SerialDeviceListModel(this))
{
	ui->setupUi(this);
	//qDebug()<<"Setting model";
	ui->listViewSerialInterface->setModel(mSerialDevices);
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
	moveTo(1);
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
	case(0): {
		selectFirstIfNoneSelected(ui->listWidgetTemplate);
	}
	break;
	case(1): {
		selectFirstIfNoneSelected(ui->listWidgetInterfaceType);
	}
	break;
	case(2): {
		selectFirstIfNoneSelected(ui->listViewSerialInterface);
	}
	break;
	case(3): {
		selectFirstIfNoneSelected(ui->listWidgetProtocol);
	}
	break;
	case(4): {
		emit done();
		//selectFirstIfNoneSelected(ui->listWidgetProtocol);
	}
	break;
	}

	ui->stackedWidget->setCurrentIndex( next );
}

void HardwareWizard::on_pushButtonOnward_clicked()
{
	const int next=mod(ui->stackedWidget->currentIndex()+1, ui->stackedWidget->count());
	moveTo(next);
}

void HardwareWizard::on_pushButtonBack_clicked()
{
	const int next=mod(ui->stackedWidget->currentIndex()-1, ui->stackedWidget->count());
	moveTo(next);
}

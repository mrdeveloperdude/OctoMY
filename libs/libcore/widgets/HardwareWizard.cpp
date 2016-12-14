#include "HardwareWizard.hpp"
#include "ui_HardwareWizard.h"

#include "SerialDeviceListModel.hpp"

#include <QDebug>




const QString HardwareTemplate::PROTOCOL_SERVOTOR32_2_1="PROTOCOL_SERVOTOR_2_1";
const QString HardwareTemplate::INTERFACE_SERVOTOR32="INTERFACE_SERVOTOR32";



HardwareTemplate::HardwareTemplate(  const QString &name, const QString &description, const QString &iconURL,const QString &interface, const QString &protocol, const quint32 poseSize)
	: mName(name)
	, mDescription(description)
	, mIconURL(iconURL)
	, mInterface(interface)
	, mProtocol(protocol)
	, mPoseSize(poseSize)

{

}



QString HardwareTemplate::name() const
{
	return mName;
}
QString HardwareTemplate::description()const
{
	return mDescription;
}

QString HardwareTemplate::iconURL()const
{
	return mIconURL;
}
QString HardwareTemplate::interface()const
{
	return mInterface;
}
QString HardwareTemplate::protocol()const
{
	return mProtocol;
}

quint32 HardwareTemplate::poseSize()const
{
	return mPoseSize;
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

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


void HardwareWizard::save()
{
	switch(ui->stackedWidget->currentIndex()) {
	//Template
	case(0): {

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

#include "HardwareWizard.hpp"
#include "ui_HardwareWizard.h"

#include "HardwareTemplate.hpp"


#include "hardware/controllers/IController.hpp"
#include "hardware/HardwareTemplateModel.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


#include <QDebug>


HardwareWizard::HardwareWizard(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::HardwareWizard)
	, mSelectedTempalte(nullptr)
	, mAgent(nullptr)
	, mHardwareTemplateModel(OC_NEW HardwareTemplateModel(this))
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->listViewTemplate->setModel(mHardwareTemplateModel);
}


HardwareWizard::~HardwareWizard()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


void HardwareWizard::on_pushButtonOnward_clicked()
{
	OC_METHODGATE();

}

void HardwareWizard::on_pushButtonBack_clicked()
{
	OC_METHODGATE();

}

void HardwareWizard::on_listWidgetController_doubleClicked(const QModelIndex &index)
{
	OC_METHODGATE();
	Q_UNUSED(index);
	on_pushButtonOnward_clicked();
}

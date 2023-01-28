#include "NetworkSettingsTestWidget.hpp"
#include "ui_NetworkSettingsTestWidget.h"


NetworkSettingsTestWidget::NetworkSettingsTestWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::NetworkSettingsTestWidget)
	, mAddresslist(new LocalAddressList)
{
	ui->setupUi(this);
	mAddresslist->configure(1337);
}


NetworkSettingsTestWidget::~NetworkSettingsTestWidget()
{
	delete ui;
}


void NetworkSettingsTestWidget::on_pushButtonConfigure_clicked()
{
	NetworkSettingsWidget *nsw=ui->widgetNetworkSettingsWidget;
	nsw->configure(mAddresslist);
}


void NetworkSettingsTestWidget::on_pushButtonToggleTimer_toggled(bool checked)
{
	mAddresslist->activate(checked);
}

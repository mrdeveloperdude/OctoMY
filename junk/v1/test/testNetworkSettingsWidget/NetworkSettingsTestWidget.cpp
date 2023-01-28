#include "NetworkSettingsTestWidget.hpp"
#include "ui_NetworkSettingsTestWidget.h"




NetworkSettingsTestWidget::NetworkSettingsTestWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::NetworkSettingsTestWidget)
	, mAddresslist(1337)
{
	ui->setupUi(this);
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
	mAddresslist.setTimerEnabled(checked);
}

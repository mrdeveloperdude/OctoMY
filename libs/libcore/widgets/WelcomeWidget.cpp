#include "WelcomeWidget.hpp"
#include "ui_WelcomeWidget.h"

#include "basic/Settings.hpp"

#include <QDesktopServices>
#include <QUrl>

WelcomeWidget::WelcomeWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WelcomeWidget)
{
	ui->setupUi(this);
}

WelcomeWidget::~WelcomeWidget()
{
	delete ui;
}

void WelcomeWidget::on_pushButtonYoutubeTutorial_clicked()
{
	QDesktopServices::openUrl(QUrl("https://www.youtube.com/watch?v=mTiqZm-Ea70", QUrl::TolerantMode));
}

void WelcomeWidget::on_pushButtonInitialize_clicked()
{
	Settings::getInstance().setCustomSettingBool("octomy.initialized",true);
	emit initialized();
}

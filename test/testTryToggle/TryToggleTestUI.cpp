#include "TryToggleTestUI.hpp"
#include "ui_TryToggleTestUI.h"

TryToggleTestUI::TryToggleTestUI(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::TryToggleTestUI)
{
	ui->setupUi(this);
	connect(ui->widgetTryToggle, SIGNAL(stateChanged(TryToggleState,TryToggleState)), this, SLOT(onStateChanged(TryToggleState,TryToggleState)));
}

TryToggleTestUI::~TryToggleTestUI()
{
	delete ui;
}

void TryToggleTestUI::on_pushButtonOff_clicked()
{
	qDebug()<<"OFF BUTTON CLICKED";
	ui->widgetTryToggle->setState(TryToggleState::OFF);
}

void TryToggleTestUI::on_pushButtonGoingOn_clicked()
{
	qDebug()<<"GOING ON BUTTON CLICKED";
	ui->widgetTryToggle->setState(TryToggleState::GOING_ON);
}

void TryToggleTestUI::on_pushButtonOn_clicked()
{
	qDebug()<<"ON BUTTON CLICKED";
	ui->widgetTryToggle->setState(TryToggleState::ON);
}

void TryToggleTestUI::on_pushButtonGoingOff_clicked()
{
	qDebug()<<"GOING OFF BUTTON CLICKED";
	ui->widgetTryToggle->setState(TryToggleState::GOING_OFF);
}




void TryToggleTestUI::onStateChanged(TryToggleState from,TryToggleState to)
{
	qDebug()<<"GOT STATE CHANGE SIGNAL FROM "<<from<<" TO "<<to;
	ui->labelLastStateEmitted->setText(ToggleStateToSTring(to));
}

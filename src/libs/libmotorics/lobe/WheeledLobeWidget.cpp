#include "WheeledLobeWidget.hpp"
#include "ui_WheeledLobeWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QInputDialog>
#include <QRegularExpression>



WheeledLobeWidget::WheeledLobeWidget(QWidget *parent)
	: LobeWidget(parent)
	, ui(OC_NEW Ui::WheeledLobeWidget)
	, mWheeledLobe(nullptr)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

WheeledLobeWidget::~WheeledLobeWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


void WheeledLobeWidget::on_pushButtonName_clicked()
{
	OC_METHODGATE();
	bool ok=false;
	QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"), tr("User name:"), QLineEdit::Normal, ui->pushButtonName->text(), &ok);
	if (ok && !text.isEmpty()) {
		text=text.replace(QRegularExpression("[^a-zA-Z0-9_]*"),"");
		//(Qt::ImhDigitsOnly|Qt::ImhUppercaseOnly|Qt::ImhLowercaseOnly)
		ui->pushButtonName->setText(text);
	}

}

void WheeledLobeWidget::on_pushButtonDelete_clicked()
{
	OC_METHODGATE();
	emit LobeDeleted(0);
}


void WheeledLobeWidget::configure(WheeledLobe *wheeledLobe)
{
	OC_METHODGATE();
	mWheeledLobe=wheeledLobe;
}

#include "WheeledMtlobeWidget.hpp"
#include "ui_WheeledMtlobeWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QInputDialog>
#include <QRegularExpression>



WheeledMtlobeWidget::WheeledMtlobeWidget(QWidget *parent)
	: MtlobeWidget(parent)
	, ui(OC_NEW Ui::WheeledMtlobeWidget)
	, mWheeledMtlobe(nullptr)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

WheeledMtlobeWidget::~WheeledMtlobeWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


void WheeledMtlobeWidget::on_pushButtonName_clicked()
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

void WheeledMtlobeWidget::on_pushButtonDelete_clicked()
{
	OC_METHODGATE();
	emit mtlobeDeleted(0);
}


void WheeledMtlobeWidget::configure(WheeledMtlobe *wheeledMtlobe)
{
	OC_METHODGATE();
	mWheeledMtlobe=wheeledMtlobe;
}

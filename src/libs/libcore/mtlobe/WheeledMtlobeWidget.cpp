#include "WheeledMtlobeWidget.hpp"
#include "ui_WheeledMtlobeWidget.h"

#include <QInputDialog>
#include <QRegularExpression>



WheeledMtlobeWidget::WheeledMtlobeWidget(QWidget *parent)
	: MtlobeWidget(parent)
	, ui(new Ui::WheeledMtlobeWidget)
	, mWheeledMtlobe(nullptr)
{
	ui->setupUi(this);
}

WheeledMtlobeWidget::~WheeledMtlobeWidget()
{
	delete ui;
}


void WheeledMtlobeWidget::on_pushButtonName_clicked()
{
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
	emit mtlobeDeleted(0);
}


void WheeledMtlobeWidget::configure(WheeledMtlobe *wheeledMtlobe)
{
	mWheeledMtlobe=wheeledMtlobe;
}

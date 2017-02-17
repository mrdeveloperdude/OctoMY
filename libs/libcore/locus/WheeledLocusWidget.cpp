#include "WheeledLocusWidget.hpp"
#include "ui_WheeledLocusWidget.h"

#include <QInputDialog>
#include <QRegularExpression>



WheeledLocusWidget::WheeledLocusWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::WheeledLocusWidget)
{
	ui->setupUi(this);
}

WheeledLocusWidget::~WheeledLocusWidget()
{
	delete ui;
}


void WheeledLocusWidget::on_pushButtonName_clicked()
{
	bool ok=false;
	QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"), tr("User name:"), QLineEdit::Normal, ui->pushButtonName->text(), &ok);
	if (ok && !text.isEmpty()) {
		text=text.replace(QRegularExpression("[^a-zA-Z0-9_]*"),"");
		//(Qt::ImhDigitsOnly|Qt::ImhUppercaseOnly|Qt::ImhLowercaseOnly)
		ui->pushButtonName->setText(text);
	}

}

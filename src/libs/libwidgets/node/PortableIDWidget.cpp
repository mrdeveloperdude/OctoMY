#include "PortableIDWidget.hpp"
#include "ui_PortableIDWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "app/Constants.hpp"

#include <QDateTime>

PortableIDWidget::PortableIDWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::PortableIDWidget)
	, mConfigureHelper("PortableIDWidget", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


PortableIDWidget::~PortableIDWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


void PortableIDWidget::configure(bool showCertificateFirst, bool userCanChange)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		ui->pushButtonToggleView->setVisible(userCanChange);
		ui->pushButtonToggleView->setChecked(showCertificateFirst);
		ui->stackedWidget->setCurrentWidget(showCertificateFirst?ui->pageCertificate:ui->pageNameplate);
	}
}


void PortableIDWidget::setPortableID(PortableID id)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		//qDebug()<<"-----------###==== PortableIDWidget was updated with "<<id;
		mID=id;
		const NodeType type=mID.type();
		const bool isAgent = TYPE_AGENT == type;
		ui->groupBoxBirthCertificate->setTitle(isAgent?"Birth Certificate": "Identification");

		ui->labelType->setVisible(true);
		ui->labelTypeCaption->setVisible(true);
		ui->labelType->setText(nodeTypeToString(type));

		ui->labelName->setVisible(!mID.name().isEmpty());
		ui->labelNameCaption->setVisible(!mID.name().isEmpty());
		ui->labelName->setText(mID.name());

		ui->labelGender->setVisible(isAgent && !mID.gender().isEmpty());
		ui->labelGenderCaption->setVisible(isAgent && !mID.gender().isEmpty());
		ui->labelGender->setText(mID.gender());

		ui->labelIDCaption->setText(isAgent?"Soul ID": "ID");
		ui->labelID->setText(mID.id());

		ui->widgetIdenticon->setPortableID(mID);
		ui->widgetQR->setQRData(mID.id());
		const quint64 ts = mID.birthDate();
		if(ts>0) {
			ui->labelBirthdate->setVisible(true);
			ui->labelBirthdateCaption->setVisible(true);
			ui->labelBirthdate->setText(QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(ts)).toString(Constants::dateFMTDay));
			ui->labelBirthdate->setToolTip(QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(ts)).toString(Constants::dateFMTMillisecond));
			ui->labelBirthdateCaption->setText(isAgent?"Birth date": "Create date");
		} else {
			ui->labelBirthdate->setVisible(false);
			ui->labelBirthdateCaption->setVisible(false);
		}

		ui->widgetBadgeIdenticon->setPortableID(id);
		ui->labelBadgeName->setText(mID.identifier());
		ui->labelBadgeName->setToolTip(mID.toPortableString());

		//ui->widgetIdenticon->setMinimumHeight(100);	ui->widgetQR->setMinimumHeight(100);
		update();
	}
}


PortableID PortableIDWidget::getPortableID()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		return mID;
	}
	return PortableID();
}


void PortableIDWidget::on_pushButtonToggleViewNameplate_clicked()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		ui->stackedWidget->setCurrentWidget(ui->pageCertificate);
	}
}


void PortableIDWidget::on_pushButtonToggleView_clicked()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		ui->stackedWidget->setCurrentWidget(ui->pageNameplate);
	}
}

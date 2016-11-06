#include "PortableIDWidget.hpp"
#include "ui_PortableIDWidget.h"

#include <QDateTime>

PortableIDWidget::PortableIDWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::PortableIDWidget)
{
	ui->setupUi(this);
	configure(true,false);
}

PortableIDWidget::~PortableIDWidget()
{
	delete ui;
}

void PortableIDWidget::configure(bool showCertificateFirst, bool userCanChange)
{
	ui->pushButtonToggleView->setVisible(userCanChange);
	ui->stackedWidget->setCurrentWidget(showCertificateFirst?ui->pageCertificate:ui->pageNameplate);
}



void PortableIDWidget::setPortableID(PortableID id)
{
	qDebug()<<"portable id widget was updated with "<<id;
	mID=id;
	const DiscoveryType type=mID.type();
	const bool isAgent=TYPE_AGENT==type;
	ui->groupBoxBirthCertificate->setTitle(isAgent?"Birth Certificate": "Identification");

	ui->labelType->setVisible(!isAgent);
	ui->labelTypeCaption->setVisible(!isAgent);
	ui->labelType->setText(DiscoveryTypeToString(type));

	ui->labelName->setVisible(isAgent && !mID.name().isEmpty());
	ui->labelNameCaption->setVisible(isAgent && !mID.name().isEmpty());
	ui->labelName->setText(mID.name());

	ui->labelGender->setVisible(isAgent && !mID.gender().isEmpty());
	ui->labelGenderCaption->setVisible(isAgent && !mID.gender().isEmpty());
	ui->labelGender->setText(mID.gender());

	ui->labelIDCaption->setText(isAgent?"Soul ID": "ID");
	ui->labelID->setText(mID.id());

	ui->widgetIdenticon->setPortableID(id);
	ui->widgetQR->setQRData(mID.id());
	const quint64 ts=mID.birthDate();
	if(ts>0) {
		ui->labelBirthdate->setVisible(true);
		ui->labelBirthdateCaption->setVisible(true);
		ui->labelBirthdate->setText(QDateTime::fromMSecsSinceEpoch(ts).toString("yyyy.MM.dd hh:mm:ss.zzz"));
		ui->labelBirthdateCaption->setText(isAgent?"Birth date": "Create date");
	} else {
		ui->labelBirthdate->setVisible(false);
		ui->labelBirthdateCaption->setVisible(false);
	}

	ui->widgetBadgeIdenticon->setPortableID(id);
	ui->labelBadgeName->setText(mID.name());
	ui->labelBadgeName->setToolTip(mID.toPortableString());

	//ui->widgetIdenticon->setMinimumHeight(100);	ui->widgetQR->setMinimumHeight(100);
	update();
}

PortableID PortableIDWidget::getPortableID()
{
	return mID;
}

void PortableIDWidget::on_pushButtonToggleView_toggled(bool checked)
{
	ui->stackedWidget->setCurrentWidget(checked?ui->pageCertificate:ui->pageNameplate);
}

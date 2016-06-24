#include "PortableIDWidget.hpp"
#include "ui_PortableIDWidget.h"

#include <QDateTime>

PortableIDWidget::PortableIDWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::PortableIDWidget)
{
	ui->setupUi(this);
}

PortableIDWidget::~PortableIDWidget()
{
	delete ui;
}

void PortableIDWidget::setPortableID(PortableID id){
	mID=id;
	const DiscoveryType type=mID.type();
	const bool isAgent=TYPE_AGENT==type;
	ui->groupBoxBirthCertificate->setTitle(isAgent?"Birth Certificate": "Identification");
	ui->labelName->setVisible(isAgent);
	ui->labelNameCaption->setVisible(isAgent);
	ui->labelGender->setVisible(isAgent);
	ui->labelGenderCaption->setVisible(isAgent);
	ui->labelIDCaption->setText(isAgent?"Soul ID": "ID");
	ui->labelBirthdateCaption->setText(isAgent?"Birth date": "Create date");
	ui->labelName->setText(mID.name());
	ui->labelGender->setText(mID.gender());
	ui->labelID->setText(mID.id());
	ui->widgetIdenticon->setIdenticonData(mID.id().toUtf8());
	ui->widgetQR->setQRData(mID.id());
	ui->labelBirthdate->setText(QDateTime::fromMSecsSinceEpoch(mID.birthDate()).toString("yyyy.MM.dd hh:mm:ss.zzz"));
}

PortableID PortableIDWidget::getPortableID(){

}

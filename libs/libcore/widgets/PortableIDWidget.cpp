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
	ui->labelName->setVisible(isAgent && !mID.name().isEmpty());
	ui->labelNameCaption->setVisible(isAgent && !mID.name().isEmpty());
	ui->labelGender->setVisible(isAgent && !mID.gender().isEmpty());
	ui->labelGenderCaption->setVisible(isAgent && !mID.gender().isEmpty());
	ui->labelIDCaption->setText(isAgent?"Soul ID": "ID");
	ui->labelName->setText(mID.name());
	ui->labelGender->setText(mID.gender());
	ui->labelID->setText(mID.id());
	ui->widgetIdenticon->setPortableID(id);
	ui->widgetQR->setQRData(mID.id());
	const quint64 ts=mID.birthDate();
	if(ts>0){
		ui->labelBirthdate->setVisible(true);
		ui->labelBirthdateCaption->setVisible(true);
		ui->labelBirthdate->setText(QDateTime::fromMSecsSinceEpoch(ts).toString("yyyy.MM.dd hh:mm:ss.zzz"));
		ui->labelBirthdateCaption->setText(isAgent?"Birth date": "Create date");
	}
	else{
		ui->labelBirthdate->setVisible(false);
		ui->labelBirthdateCaption->setVisible(false);
	}

	//ui->widgetIdenticon->setMinimumHeight(100);	ui->widgetQR->setMinimumHeight(100);
	update();
}

PortableID PortableIDWidget::getPortableID(){

}

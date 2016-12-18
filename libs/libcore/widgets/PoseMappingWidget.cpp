#include "PoseMappingWidget.hpp"
#include "ui_PoseMappingWidget.h"

#include "../libutil/utility/Utility.hpp"
#include "puppet/PoseMapping.hpp"
#include "widgets/PoseMappingView.hpp"

#include <QPushButton>
#include <QLayout>
#include <QSpacerItem>
#include <QDebug>
#include <QButtonGroup>

PoseMappingWidget::PoseMappingWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::PoseMappingWidget)
	, mMapping(nullptr)
	, butGroupFrom(nullptr)
	, butGroupTo(nullptr)
	, fromButton(nullptr)
	, toButton(nullptr)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

PoseMappingWidget::~PoseMappingWidget()
{
	OC_METHODGATE();
	delete ui;
}


void PoseMappingWidget::configure(PoseMapping &mapping)
{
	OC_METHODGATE();
	mMapping=&mapping;
	ui->verticalLayout_2->removeWidget(ui->widgetPoses);
	ui->widgetPoses->deleteLater();
	ui->widgetPoses = new PoseMappingView(ui->scrollAreaWidgetContents);
	ui->widgetPoses->setObjectName(QStringLiteral("widgetPoses"));
	butGroupFrom=new QButtonGroup(ui->widgetPoses);
	butGroupTo=new QButtonGroup(ui->widgetPoses);

	fromButton=nullptr;
	toButton=nullptr;

	connect(butGroupFrom, static_cast<void(QButtonGroup::*)(QAbstractButton *, bool)>(&QButtonGroup::buttonToggled),
	[=](QAbstractButton *button, bool checked) {
		fromButton=button;
		makeConnection();
	});

	connect(butGroupTo, static_cast<void(QButtonGroup::*)(QAbstractButton *, bool)>(&QButtonGroup::buttonToggled),
	[=](QAbstractButton *button, bool checked) {
		toButton=button;
		makeConnection();
	});
	QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(ui->widgetPoses->sizePolicy().hasHeightForWidth());
	ui->widgetPoses->setSizePolicy(sizePolicy);
	QVBoxLayout *hLayout = new QVBoxLayout(ui->widgetPoses);

	if(nullptr!=mMapping) {
		const quint32 sz=mMapping->size();
		for(quint32 i=0; i<sz; ++i) {
			//addButtonPair(hLayout, "Anna","Jeanette", butGroupFrom, butGroupTo);
			addButtonPair(hLayout, mMapping->name(i), QString("Servo_%1").arg(i), butGroupFrom, butGroupTo);
		}
	}
	QSpacerItem *verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
	hLayout->addItem(verticalSpacer);
	ui->verticalLayout_2->addWidget(ui->widgetPoses);
	ui->widgetPoses->configure(*this);
}

PoseMapping *PoseMappingWidget::mapping()
{
	OC_METHODGATE();
	return mMapping;
}

void PoseMappingWidget::addButtonPair(QVBoxLayout *hLayout, QString from,QString to, QButtonGroup *butGroupFrom, QButtonGroup *butGroupTo)
{
	OC_METHODGATE();
	QHBoxLayout *horizontalLayoutButtons1 = new QHBoxLayout();
	QPushButton*pushButtonFrom = new QPushButton(ui->widgetPoses);
	pushButtonFrom->setObjectName("pushButtonFrom");
	pushButtonFrom->setText(from);
	pushButtonFrom->setCheckable(true);
	butGroupFrom->addButton(pushButtonFrom);
	horizontalLayoutButtons1->addWidget(pushButtonFrom);
	QSpacerItem*horizontalSpacerButtons1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	horizontalLayoutButtons1->addItem(horizontalSpacerButtons1);
	QPushButton *pushButtonTo = new QPushButton(ui->widgetPoses);
	pushButtonTo->setObjectName("pushButtonTo");
	pushButtonTo->setText(to);
	pushButtonTo->setCheckable(true);
	butGroupTo->addButton(pushButtonTo);
	horizontalLayoutButtons1->addWidget(pushButtonTo);
	hLayout->addLayout(horizontalLayoutButtons1);
}


void PoseMappingWidget::makeConnection()
{
	OC_METHODGATE();
	if(nullptr!=fromButton && nullptr!=toButton) {
		qint32 fromIndex=utility::getSelectedButtonIndex(butGroupFrom,-1);
		qint32 toIndex=utility::getSelectedButtonIndex(butGroupTo,-1);
		if(fromIndex>=0 && toIndex >=0) {
			//qDebug()<<"SETTING MAPPING from "<<fromIndex<<" to "<<toIndex;
			mMapping->setMapping(fromIndex,toIndex, true);
		}
		utility::clearButtonGroupSelection(butGroupFrom);
		utility::clearButtonGroupSelection(butGroupTo);
		fromButton=nullptr;
		toButton=nullptr;
		update();
		ui->widgetPoses->update();
	}
}

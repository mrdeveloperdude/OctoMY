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
#include <QInputDialog>

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
	if(!connect(ui->spinBoxActuatorCount, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &PoseMappingWidget::onSpinValueChanged, OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}
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
		ui->spinBoxActuatorCount->setValue(sz);
		for(quint32 i=0; i<sz; ++i) {
			//addButtonPair(hLayout, "Anna","Jeanette", butGroupFrom, butGroupTo);
			addButtonPair(i,hLayout, mMapping->name(i), QString("Servo_%1").arg(i), butGroupFrom, butGroupTo);
		}
	} else {
		ui->spinBoxActuatorCount->setValue(0);
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

void PoseMappingWidget::addButtonPair(quint32 index, QVBoxLayout *hLayout, QString from,QString to, QButtonGroup *butGroupFrom, QButtonGroup *butGroupTo)
{
	OC_METHODGATE();
	QHBoxLayout *horizontalLayoutButtons = new QHBoxLayout();
	QPushButton*pushButtonRename = new QPushButton(ui->widgetPoses);
	pushButtonRename->setObjectName("pushButtonRename");
	pushButtonRename->setText("Rename");
	horizontalLayoutButtons->addWidget(pushButtonRename);


	QPushButton*pushButtonFrom = new QPushButton(ui->widgetPoses);
	pushButtonFrom->setObjectName("pushButtonFrom");
	pushButtonFrom->setText(from);
	pushButtonFrom->setCheckable(true);
	butGroupFrom->addButton(pushButtonFrom);
	horizontalLayoutButtons->addWidget(pushButtonFrom);
	QSpacerItem*horizontalSpacerButtons1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	horizontalLayoutButtons->addItem(horizontalSpacerButtons1);
	QPushButton *pushButtonTo = new QPushButton(ui->widgetPoses);
	pushButtonTo->setObjectName("pushButtonTo");
	pushButtonTo->setText(to);
	pushButtonTo->setCheckable(true);
	butGroupTo->addButton(pushButtonTo);
	horizontalLayoutButtons->addWidget(pushButtonTo);
	hLayout->addLayout(horizontalLayoutButtons);

	if(!connect(pushButtonRename,  static_cast<void(QPushButton::*)(bool)>(&QPushButton::clicked), this, [=](bool) {
	bool ok=false;
	QString oldName=mMapping->name(index);
		QString name= QInputDialog::getText(this, tr("Rename"), tr("Please enter new name:"), QLineEdit::Normal,oldName, &ok);
		if (ok && !name.isEmpty()) {
			qDebug()<<"RENAME SUCCESSFULL: "<<name;
			mMapping->setName(index,name);
			if(nullptr!=pushButtonFrom) {
				pushButtonFrom->setText(name);
			}
		}

	}, OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect";
	}

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


void PoseMappingWidget::onSpinValueChanged(int size)
{
	if(nullptr!=mMapping) {
		qDebug()<<"RESIZING TO: "<<size;
		mMapping->resize(size);
		configure(*mMapping);
	}
}

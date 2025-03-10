#include "PoseMappingWidget.hpp"
#include "ui_PoseMappingWidget.h"

#include "pose/PoseMapping.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "utility/ui/Ui.hpp"
#include "PoseMappingView.hpp"

#include <QPushButton>
#include <QLayout>
#include <QSpacerItem>
#include <QDebug>
#include <QButtonGroup>
#include <QInputDialog>

PoseMappingWidget::PoseMappingWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::PoseMappingWidget)
	, mMapping(nullptr)
	, butGroupFrom(nullptr)
	, butGroupTo(nullptr)
	, fromButton(nullptr)
	, toButton(nullptr)
{
	OC_METHODGATE();
	ui->setupUi(this);
	if(!connect(ui->spinBoxActuatorCount, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &PoseMappingWidget::onSpinValueChanged, OC_CONTYPE)) {
		qWarning() << "ERROR: Could not connect";
	}
}


PoseMappingWidget::~PoseMappingWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


void PoseMappingWidget::configure(QSharedPointer<PoseMapping> mapping)
{
	OC_METHODGATE();
	mMapping = mapping;
	ui->verticalLayout_2->removeWidget(ui->widgetPoses);
	ui->widgetPoses->deleteLater();
	ui->widgetPoses = OC_NEW PoseMappingView(ui->scrollAreaWidgetContents);
	ui->widgetPoses->setObjectName(QStringLiteral("widgetPoses"));
	butGroupFrom=OC_NEW QButtonGroup(ui->widgetPoses);
	butGroupTo=OC_NEW QButtonGroup(ui->widgetPoses);

	fromButton=nullptr;
	toButton=nullptr;

	connect(butGroupFrom, static_cast<void(QButtonGroup::*)(QAbstractButton *, bool)>(&QButtonGroup::buttonToggled),
	[=](QAbstractButton *button, bool checked) {
		Q_UNUSED(checked);
		fromButton=button;
		makeConnection();
	});

	connect(butGroupTo, static_cast<void(QButtonGroup::*)(QAbstractButton *, bool)>(&QButtonGroup::buttonToggled),
	[=](QAbstractButton *button, bool checked) {
		Q_UNUSED(checked);
		toButton=button;
		makeConnection();
	});
	QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(ui->widgetPoses->sizePolicy().hasHeightForWidth());
	ui->widgetPoses->setSizePolicy(sizePolicy);
	QVBoxLayout *hLayout = OC_NEW QVBoxLayout(ui->widgetPoses);

	if(nullptr!=mMapping) {
		const quint32 sz=mMapping->size();
		ui->spinBoxActuatorCount->setValue(static_cast<int>(sz));
		for(quint32 i=0; i<sz; ++i) {
			//addButtonPair(hLayout, "Anna","Jeanette", butGroupFrom, butGroupTo);
			addButtonPair(i,hLayout, mMapping->name(i), QString("Servo_%1").arg(i), butGroupFrom, butGroupTo);
		}
	} else {
		ui->spinBoxActuatorCount->setValue(0);
	}
	QSpacerItem *verticalSpacer = OC_NEW QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
	hLayout->addItem(verticalSpacer);
	ui->verticalLayout_2->addWidget(ui->widgetPoses);
	ui->widgetPoses->configure(*this);
}

QSharedPointer<PoseMapping> PoseMappingWidget::mapping() const
{
	OC_METHODGATE();
	return mMapping;
}

void PoseMappingWidget::addButtonPair(quint32 index, QVBoxLayout *hLayout, QString from,QString to, QButtonGroup *butGroupFrom, QButtonGroup *butGroupTo)
{
	OC_METHODGATE();
	QHBoxLayout *horizontalLayoutButtons = OC_NEW QHBoxLayout();
	QPushButton*pushButtonRename = OC_NEW QPushButton(ui->widgetPoses);
	pushButtonRename->setObjectName("pushButtonRename");
	pushButtonRename->setText("Rename");
	horizontalLayoutButtons->addWidget(pushButtonRename);


	QPushButton*pushButtonFrom = OC_NEW QPushButton(ui->widgetPoses);
	pushButtonFrom->setObjectName("pushButtonFrom");
	pushButtonFrom->setText(from);
	pushButtonFrom->setCheckable(true);
	butGroupFrom->addButton(pushButtonFrom);
	horizontalLayoutButtons->addWidget(pushButtonFrom);
	QSpacerItem*horizontalSpacerButtons1 = OC_NEW QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	horizontalLayoutButtons->addItem(horizontalSpacerButtons1);
	QPushButton *pushButtonTo = OC_NEW QPushButton(ui->widgetPoses);
	pushButtonTo->setObjectName("pushButtonTo");
	pushButtonTo->setText(to);
	pushButtonTo->setCheckable(true);
	butGroupTo->addButton(pushButtonTo);
	horizontalLayoutButtons->addWidget(pushButtonTo);
	hLayout->addLayout(horizontalLayoutButtons);

	if(!connect(pushButtonRename,  static_cast<void(QPushButton::*)(bool)>(&QPushButton::clicked), this, [=](bool) {
		bool ok=false;
		if(!mMapping.isNull()){
			QString oldName = mMapping->name(index);
				QString name= QInputDialog::getText(this, tr("Rename"), tr("Please enter new name:"), QLineEdit::Normal,oldName, &ok);
				if (ok && !name.isEmpty()) {
					if(mDebug){
						qDebug() << "RENAME SUCCESSFULL: " << name;
					}
					mMapping->setName(index,name);
					if(nullptr!=pushButtonFrom) {
						pushButtonFrom->setText(name);
					}
				}
		}
		else{
			qWarning() << "ERROR: no mapping";
		}
	}, OC_CONTYPE_NON_UNIQUE)) {
		qWarning() << "ERROR: Could not connect";
	}
}


void PoseMappingWidget::makeConnection()
{
	OC_METHODGATE();
	if(nullptr!=fromButton && nullptr!=toButton) {
		const qint32 fromIndex=utility::ui::selectedButtonIndex(butGroupFrom,-1);
		const qint32 toIndex=utility::ui::selectedButtonIndex(butGroupTo,-1);
		if(fromIndex>=0 && toIndex >=0) {
			if(mDebug){
				qDebug() << "SETTING MAPPING from " << fromIndex << " to " << toIndex;
			}
			if(!mMapping.isNull()){
				mMapping->setMapping(static_cast<quint32>(fromIndex), static_cast<quint32>(toIndex), true);
			}
			else{
				qWarning() << "ERROR: no mapping";
			}
		}
		utility::ui::clearButtonGroupSelection(butGroupFrom);
		utility::ui::clearButtonGroupSelection(butGroupTo);
		fromButton=nullptr;
		toButton=nullptr;
		update();
		ui->widgetPoses->update();
	}
}


void PoseMappingWidget::onSpinValueChanged(int size)
{
	OC_METHODGATE();
	if(!mMapping.isNull()) {
		if(mDebug){
			qDebug() << "RESIZING TO: " << size;
		}
		mMapping->resize(static_cast<quint32>(size));
		configure(mMapping);
	}
}

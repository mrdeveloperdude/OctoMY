#include "NameMappingWidget.hpp"
#include "ui_NameMappingWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "utility/ui/Ui.hpp"

#include <QPushButton>
#include <QLayout>
#include <QSpacerItem>
#include <QDebug>
#include <QButtonGroup>
#include <QInputDialog>
#include <QLabel>

NameMappingWidget::NameMappingWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::NameMappingWidget)
	, mButGroupFrom(nullptr)
	, mButGroupTo(nullptr)
	, mCurrentFromButton(nullptr)
	, mCurrentToButton(nullptr)
	, mHumble(QSizePolicy::Minimum,QSizePolicy::Minimum)
{
	OC_METHODGATE();
	ui->setupUi(this);

	mHumble.setHorizontalStretch(0);
	mHumble.setVerticalStretch(0);
	mHumble.setRetainSizeWhenHidden(true);

	mButGroupFrom=OC_NEW QButtonGroup(ui->widgetNames);
	mButGroupTo=OC_NEW QButtonGroup(ui->widgetNames);

	if(!connect(mButGroupFrom, static_cast<void(QButtonGroup::*)(QAbstractButton *, bool)>(&QButtonGroup::buttonToggled),
	[=](QAbstractButton *button, bool) {
	mCurrentFromButton=button;
	makeConnection();
	})) {
		qDebug()<<"ERROR: could not connect";
	};

	if(!connect(mButGroupTo, static_cast<void(QButtonGroup::*)(QAbstractButton *, bool)>(&QButtonGroup::buttonToggled),
	[=](QAbstractButton *button, bool) {
	mCurrentToButton=button;
	makeConnection();
	})) {
		qDebug()<<"ERROR: could not connect";
	};
}


NameMappingWidget::~NameMappingWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}
/*
void bubba()
{
OC_FUNCTIONGATE();

	scrollArea = OC_NEW QScrollArea(NameMappingWidget);
	scrollArea->setObjectName(QStringLiteral("scrollArea"));
	scrollArea->setWidgetResizable(true);
	scrollAreaWidgetContents = OC_NEW QWidget();
	scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
	scrollAreaWidgetContents->setGeometry(QRect(0, 0, 380, 280));
	verticalLayout_2 = OC_NEW QVBoxLayout(scrollAreaWidgetContents);
	verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
	widgetNames = OC_NEW NameMappingView(scrollAreaWidgetContents);
	widgetNames->setObjectName(QStringLiteral("widgetNames"));
	QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(widgetNames->sizePolicy().hasHeightForWidth());
	widgetNames->setSizePolicy(sizePolicy);
	gridLayout = OC_NEW QGridLayout(widgetNames);
	gridLayout->setObjectName(QStringLiteral("gridLayout"));
	label_2 = OC_NEW QLabel(widgetNames);
	label_2->setObjectName(QStringLiteral("label_2"));
	label_2->setAlignment(Qt::AlignCenter);

	gridLayout->addWidget(label_2, 0, 0, 1, 1);


	verticalLayout_2->addWidget(widgetNames);

	scrollArea->setWidget(scrollAreaWidgetContents);

	verticalLayout->addWidget(scrollArea);





	horizontalLayoutRow1 = OC_NEW QHBoxLayout();
	horizontalLayoutRow1->setObjectName(QStringLiteral("horizontalLayoutRow1"));
	pushButtonFrom1 = OC_NEW QPushButton(scrollAreaWidgetContents);
	pushButtonFrom1->setObjectName(QStringLiteral("pushButtonFrom1"));

	horizontalLayoutRow1->addWidget(pushButtonFrom1);

	horizontalSpacer1 = OC_NEW QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	horizontalLayoutRow1->addItem(horizontalSpacer1);

	pushButtonTo1 = OC_NEW QPushButton(scrollAreaWidgetContents);
	pushButtonTo1->setObjectName(QStringLiteral("pushButtonTo1"));

	horizontalLayoutRow1->addWidget(pushButtonTo1);


	verticalLayoutScrollArea->addLayout(horizontalLayoutRow1);

	horizontalLayoutRow2 = OC_NEW QHBoxLayout();
	horizontalLayoutRow2->setObjectName(QStringLiteral("horizontalLayoutRow2"));
	pushButtonFrom2 = OC_NEW QPushButton(scrollAreaWidgetContents);
	pushButtonFrom2->setObjectName(QStringLiteral("pushButtonFrom2"));

	horizontalLayoutRow2->addWidget(pushButtonFrom2);

	horizontalSpacer2 = OC_NEW QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	horizontalLayoutRow2->addItem(horizontalSpacer2);

	pushButtonTo2 = OC_NEW QPushButton(scrollAreaWidgetContents);
	pushButtonTo2->setObjectName(QStringLiteral("pushButtonTo2"));

	horizontalLayoutRow2->addWidget(pushButtonTo2);


	verticalLayoutScrollArea->addLayout(horizontalLayoutRow2);

	verticalSpacerBottom = OC_NEW QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	verticalLayoutScrollArea->addItem(verticalSpacerBottom);

	scrollArea->setWidget(scrollAreaWidgetContents);

	verticalLayoutMain->addWidget(scrollArea);



}*/

void NameMappingWidget::configure(QSharedPointer<NameMapping> mapping, QStringList fromList,QStringList toList)
{
	OC_METHODGATE();
	mMapping=mapping;
	mFromList=fromList;
	mToList=toList;

	mCurrentFromButton=nullptr;
	mCurrentToButton=nullptr;

	ui->verticalLayoutScrollAreaContent->removeWidget(ui->widgetNames);
	ui->widgetNames->deleteLater();
	ui->widgetNames = OC_NEW NameMappingView(ui->scrollAreaWidgetContents);
	ui->widgetNames->setObjectName(QStringLiteral("widgetNames"));

	{
		QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		//sizePolicy.setHeightForWidth(ui->widgetNames->sizePolicy().hasHeightForWidth());
		sizePolicy.setRetainSizeWhenHidden(true);
		ui->widgetNames->setSizePolicy(sizePolicy);
	}

	mButGroupFrom=OC_NEW QButtonGroup(ui->widgetNames);
	mButGroupTo=OC_NEW QButtonGroup(ui->widgetNames);

	//mButGroupFrom->setParent(ui->widgetNames);	mButGroupTo->setParent(ui->widgetNames);


	QVBoxLayout *hLayout = OC_NEW QVBoxLayout();


	const quint32 fromSZ=static_cast<quint32>(mFromList.size());
	const quint32 toSZ=static_cast<quint32>(mToList.size());
	const quint32 sz=qMax(fromSZ, toSZ);

	qDebug()<<"---- From SZ:"<<fromSZ<<", ToSZ:"<<toSZ<<", end SZ:"<<sz<<"";
	/*
		for(QHBoxLayout *row:mRows) {
			if(nullptr!=row) {
				row->deleteLater();
			}
		}
		*/
	mRows.clear();


	for(quint32 i=0; i<sz; ++i) {
		qDebug()<<"---- ROW: "<<i<<"/"<<sz;
		QHBoxLayout *row = OC_NEW QHBoxLayout();
		{
			QLabel *lab=OC_NEW QLabel();
			lab->setText(QString("#%1/%2").arg(i).arg(sz));
			lab->setSizePolicy(mHumble);
			lab->setMinimumSize(0,0);
			lab->setParent(ui->widgetNames);
			row->addWidget(lab);

		}

		{
			QPushButton *but = OC_NEW QPushButton();
			but->setObjectName(QString("pushButtonFrom_%1").arg(i));
			but->setCheckable(true);
			but->setMinimumSize(0,0);
			but->setSizePolicy(mHumble);
			but->setParent(ui->widgetNames);
			mButGroupFrom->addButton(but);
			row->addWidget(but);
		}
		{
			QSpacerItem *spacer= OC_NEW QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
			row->addItem(spacer);

		}
		{
			QPushButton *but= OC_NEW QPushButton();
			but->setObjectName(QString("pushButtonTo_%1").arg(i));
			but->setCheckable(true);
			but->setMinimumSize(0,0);
			but->setSizePolicy(mHumble);
			but->setParent(ui->widgetNames);
			mButGroupTo->addButton(but);
			row->addWidget(but);
		}
		mRows.push_back(row);
		hLayout->addLayout(row);
		row->setParent(hLayout);
	}

	// First add mapped values ordered by from string in provided mapping
	QStringList keys=mMapping->keys();
	QStringList values=mMapping->values();
	quint32 fromIndex=0;
	quint32 toIndex=0;
	for(QString key:keys) {
		if(mFromList.contains(key)) {
			QString val=(*mMapping)[key];
			if(mToList.contains(val) || val.isEmpty()) {
				qDebug().noquote().nospace()<<" + mapped from button "<<key<<"@"<<fromIndex<<" to "<< val<<"@"<<toIndex;
				addFromButton(key,fromIndex);
				fromIndex++;
				addToButton(val, toIndex);
				toIndex++;
			}
		}
	}

	// Add unmapped from-strings in provided list order
	for(QString from:mFromList) {
		if(!keys.contains(from)) {
			qDebug().noquote().nospace()<<" + un-mapped from button "<<from<<"@"<<fromIndex;
			addFromButton(from, fromIndex);
			fromIndex++;
		}
	}
	// Add unmapped to-strings in provided list order
	for(QString to:mToList) {
		if(!values.contains(to)) {
			qDebug().noquote().nospace()<<" + un-mapped to button "<<to<<"@"<<toIndex;
			addToButton(to, toIndex);
			toIndex++;
		}
	}

	for(; fromIndex<sz; ++fromIndex) {
		qDebug().noquote().nospace()<<" + nil from button @"<<fromIndex;
		addFromButton("",fromIndex);
	}
	for(; toIndex<sz; ++toIndex) {
		qDebug().noquote().nospace()<<" + nil to button @"<<toIndex;
		addToButton("",toIndex);
	}

	QSpacerItem *verticalSpacer = OC_NEW QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
	hLayout->addItem(verticalSpacer);
	hLayout->setParent(ui->widgetNames);
	ui->verticalLayoutScrollAreaContent->addWidget(ui->widgetNames);
	ui->widgetNames->setLayout(hLayout);
	ui->widgetNames->configure(*this);
}

QSharedPointer<NameMapping> NameMappingWidget::mapping()
{
	OC_METHODGATE();
	return mMapping;
}

QStringList &NameMappingWidget::fromList()
{
	OC_METHODGATE();
	return mFromList;
}

QStringList &NameMappingWidget::toList()
{
	OC_METHODGATE();
	return mToList;
}

QHBoxLayout *NameMappingWidget::row(quint32 r)
{
	OC_METHODGATE();
	if(r>=mRows.size()) {
		return nullptr;
	}
	return mRows[r];
}

void NameMappingWidget::addFromButton(QString from, quint32 r)
{
	OC_METHODGATE();
	QHBoxLayout *rh=row(r);
	if(nullptr!=rh) {
		auto it=rh->itemAt(1);
		if(nullptr!=it) {
			QPushButton *but=qobject_cast<QPushButton*>(it->widget());
			if(nullptr!=but) {
				but->setText(from);
				but->setVisible(!from.isEmpty());
			}
		}
	}
}


void NameMappingWidget::addToButton(QString to, quint32 r)
{
	OC_METHODGATE();
	QHBoxLayout *rh=row(r);
	if(nullptr!=rh) {
		auto it=rh->itemAt(3);
		if(nullptr!=it) {
			QPushButton *but=qobject_cast<QPushButton*>(it->widget());
			if(nullptr!=but) {
				but->setText(to);
				but->setVisible(!to.isEmpty());
			}
		}
	}

}


void NameMappingWidget::makeConnection()
{
	OC_METHODGATE();
	if(nullptr!=mCurrentFromButton && nullptr!=mCurrentToButton) {
		QAbstractButton *selectedFromBut=mButGroupFrom->checkedButton();
		QAbstractButton *selectedToBut=mButGroupTo->checkedButton();
		if(nullptr!=selectedFromBut && nullptr!= selectedToBut) {
			QString from=selectedFromBut->text();
			QString to=selectedToBut->text();

			qDebug()<<"SETTING MAPPING from "<<from<<" to "<<to;
			(*mMapping)[from]=to;
		}
		/*
		if(fromIndex>=0 && toIndex >=0) {
			//qDebug()<<"SETTING MAPPING from "<<fromIndex<<" to "<<toIndex;
			mMapping->setMapping(fromIndex,toIndex, true);
		}
		*/
		utility::ui::clearButtonGroupSelection(mButGroupFrom);
		utility::ui::clearButtonGroupSelection(mButGroupTo);
		mCurrentFromButton=nullptr;
		mCurrentToButton=nullptr;
		update();
		ui->widgetNames->configure(*this);
	}
}

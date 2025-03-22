#include "MultiView.hpp"
#include "ui_MultiView.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "uptime/ConnectionType.hpp"

#include "app/Settings.hpp"

#include <QDebug>

// Inspired by http://doc.qt.io/qt-5/qtwidgets-itemviews-interview-example.html

MultiView::MultiView(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::MultiView)
	, settings(nullptr)
{
	OC_METHODGATE();
	ui->setupUi(this);
	QListView *list = ui->listView;
	if(nullptr != list) {
		list->setViewMode(QListView::IconMode);
		list->setSelectionMode(QAbstractItemView::ExtendedSelection);
		list->setAlternatingRowColors(false);
		list->viewport()->setAttribute(Qt::WA_StaticContents);
		list->setAttribute(Qt::WA_MacShowFocusRect, false);
	}
	QTableView *table = ui->tableView;
	if(nullptr != table) {
		table->horizontalHeader()->setSectionsMovable(true);
		table->verticalHeader()->setSectionsMovable(true);
		// Set StaticContents to enable minimal repaints on resizes.
		table->viewport()->setAttribute(Qt::WA_StaticContents);
	}
	QTreeView *tree = ui->treeView;
	if(nullptr != tree) {
		tree->setUniformRowHeights(true);
		tree->header()->setStretchLastSection(false);
		tree->viewport()->setAttribute(Qt::WA_StaticContents);
		// Disable the focus rect to get minimal repaints when scrolling on Mac.
		tree->setAttribute(Qt::WA_MacShowFocusRect, false);
	}
	//Start of with no data
	setModel(nullptr);
	if(!connect(ui->buttonGroupView, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onViewButtonClicked(QAbstractButton*)), OC_CONTYPE)) {
		qDebug()<<"ERROR: could not connect";
	}
	//Trigger change
	ui->pushButtonViewList->click();
}


MultiView::~MultiView()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


void MultiView::setModel(QAbstractItemModel *data)
{
	OC_METHODGATE();
	QItemSelectionModel *selections = nullptr==data?nullptr:OC_NEW QItemSelectionModel(data);
	QListView *list = ui->listView;
	if(nullptr != list) {
		QItemSelectionModel *oldSel = list->selectionModel();
		list->setModel(data);
		delete oldSel;
		oldSel=nullptr;
		if(nullptr!=selections) {
			list->setSelectionModel(selections);
		}
	}
	QTableView *table = ui->tableView;
	if(nullptr != table) {
		QItemSelectionModel *oldSel = table->selectionModel();
		table->setModel(data);
		delete oldSel;
		oldSel=nullptr;
		if(nullptr!=selections) {
			table->setSelectionModel(selections);
		}
	}
	QTreeView *tree = ui->treeView;
	if(nullptr != tree) {
		QItemSelectionModel *oldSel = tree->selectionModel();
		tree->setModel(data);
		delete oldSel;
		oldSel=nullptr;
		if(nullptr!=selections) {
			tree->setSelectionModel(selections);
		}
	}
}


void MultiView::onViewButtonClicked(QAbstractButton*but)
{
	OC_METHODGATE();
	QAbstractButton*c=ui->buttonGroupView->checkedButton();
	QString item="";
	if(c!=but && nullptr != but) {
		qDebug()<<"CLICKING BUTTON: "<<but->objectName();
		but->click();
	}
	if(but==ui->pushButtonViewGrid) {
		ui->stackedWidget->setCurrentWidget(ui->pageList);
		//ui->listView->clearPropertyFlags();
		ui->listView->setViewMode(QListView::IconMode);
		item="Grid";
	} else if(but==ui->pushButtonViewList) {
		ui->stackedWidget->setCurrentWidget(ui->pageList);
		//ui->listView->clearPropertyFlags();
		ui->listView->setViewMode(QListView::ListMode);
		item="List";
	} else if(but==ui->pushButtonViewDetails) {
		ui->stackedWidget->setCurrentWidget(ui->pageTable);
		item="Details";
	} else if(but==ui->pushButtonViewTree) {
		ui->stackedWidget->setCurrentWidget(ui->pageTree);
		item="Tree";
	} else {
		qDebug()<<"ERROR: Unknwon button click detected";
		return;
	}
	if(nullptr!=settings) {
		settings->setCustomSetting(k,item);
	}
}



void MultiView::configure(QSharedPointer<Settings> settings, QString val, QString key)
{
	OC_METHODGATE();

	k = key.trimmed();
	if("" != k && nullptr != settings) {
		val=settings->getCustomSetting(k,val);
	}
	QAbstractButton *but=nullptr;
	if("Grid" == val) {
		but = ui->pushButtonViewGrid;
	} else if("List" == val) {
		but = ui->pushButtonViewList;
	} else if("Details" == val) {
		but = ui->pushButtonViewDetails;
	} else if("Tree" == val) {
		but = ui->pushButtonViewTree;
	} else {
		qWarning() << "ERROR: NO SUITABLE BUTTON NAME FOUND FOR " << val;
		return;
	}
	// TODO: Should this be enabled?
	Q_UNUSED(but);
	//onViewButtonClicked(but);
}



#include "MultiView.hpp"
#include "ui_MultiView.h"

#include "app/Settings.hpp"
#include "hub/MultiViewFilter.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QAbstractItemView>
#include <QDebug>
#include <QHeaderView>
#include <QListView>
#include <QPushButton>
#include <QTableView>
#include <QTreeView>
#include <QWheelEvent>

const static QString GRID_KEY("Grid");
const static QString LIST_KEY("List");
const static QString DETAILS_KEY("Details");
const static QString TREE_KEY("Tree");

MultiView::MultiView(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::MultiView)
	, mFilter{OC_NEW MultiViewFilter(this)}
	, settings(nullptr)
	, m_zoomFactor(1.0)
{
	OC_METHODGATE();
	ui->setupUi(this);
	
	QListView *list = ui->listView;
	if (nullptr != list) {
		list->setViewMode(QListView::IconMode);
		list->setSelectionMode(QAbstractItemView::ExtendedSelection);
		list->setAlternatingRowColors(false);
		list->viewport()->setAttribute(Qt::WA_StaticContents);
		list->setAttribute(Qt::WA_MacShowFocusRect, false);
		// Set a base icon size.
		list->setIconSize(QSize(64, 64));
	}
	QTableView *table = ui->tableView;
	if (nullptr != table) {
		table->horizontalHeader()->setSectionsMovable(true);
		table->verticalHeader()->setSectionsMovable(true);
		table->viewport()->setAttribute(Qt::WA_StaticContents);
		// Set a base icon size.
		table->setIconSize(QSize(64, 64));
		// Make columns stretch to fill the available width.
		table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	}
	QTreeView *tree = ui->treeView;
	if (nullptr != tree) {
		tree->setUniformRowHeights(true);
		tree->header()->setStretchLastSection(false);
		tree->viewport()->setAttribute(Qt::WA_StaticContents);
		tree->setAttribute(Qt::WA_MacShowFocusRect, false);
		// Set a base icon size.
		tree->setIconSize(QSize(64, 64));
		// Stretch all columns in the tree view.
		tree->header()->setSectionResizeMode(QHeaderView::Stretch);
	}
	setModel(nullptr);
	if(ui->listView){
		ui->listView->setModel(mFilter);
	}
	if(ui->tableView){
		ui->tableView->setModel(mFilter);
	}
	if(ui->treeView){
		ui->treeView->setModel(mFilter);
	}
	if (!connect(ui->buttonGroupView, SIGNAL(buttonClicked(QAbstractButton*)),
				 this, SLOT(onViewButtonClicked(QAbstractButton*)), OC_CONTYPE)) {
		qDebug() << "ERROR: could not connect";
	}
	ui->pushButtonViewList->click();
}


MultiView::~MultiView()
{
	OC_METHODGATE();
	delete ui;
	ui = nullptr;
}

void MultiView::setModel(QAbstractItemModel *model)
{
	OC_METHODGATE();
	auto selections = (nullptr == model ? nullptr : OC_NEW QItemSelectionModel(model));
	mFilter->setSourceModel(model);
	if (ui->listView != nullptr) {
		auto oldSel = ui->listView->selectionModel();
		delete oldSel;
		oldSel = nullptr;
		if (nullptr != selections){
			ui->listView->setSelectionModel(selections);
		}
	}
	if (ui->tableView != nullptr) {
		auto oldSel = ui->tableView->selectionModel();
		ui->tableView->setModel(model);
		delete oldSel;
		oldSel = nullptr;
		if (nullptr != selections){
			ui->tableView->setSelectionModel(selections);
		}
	}
	if (ui->treeView != nullptr) {
		auto oldSel = ui->treeView->selectionModel();
		ui->treeView->setModel(model);
		delete oldSel;
		oldSel = nullptr;
		if (nullptr != selections){
			ui->treeView->setSelectionModel(selections);
		}
	}
}

void MultiView::onViewButtonClicked(QAbstractButton* but)
{
	OC_METHODGATE();
	QAbstractButton *c = ui->buttonGroupView->checkedButton();
	QString item = "";
	if (c != but && but != nullptr) {
		qDebug() << "CLICKING BUTTON: " << but->objectName();
		but->click();
	}
	if (but == ui->pushButtonViewGrid) {
		ui->stackedWidget->setCurrentWidget(ui->pageList);
		ui->listView->setViewMode(QListView::IconMode);
		item = GRID_KEY;
	} else if (but == ui->pushButtonViewList) {
		ui->stackedWidget->setCurrentWidget(ui->pageList);
		ui->listView->setViewMode(QListView::ListMode);
		item = LIST_KEY;
	} else if (but == ui->pushButtonViewDetails) {
		ui->stackedWidget->setCurrentWidget(ui->pageTable);
		item = DETAILS_KEY;
	} else if (but == ui->pushButtonViewTree) {
		ui->stackedWidget->setCurrentWidget(ui->pageTree);
		item = TREE_KEY;
	} else {
		qDebug() << "ERROR: Unknown button click detected";
		return;
	}
	if (settings != nullptr) {
		settings->setCustomSetting(k, item);
	}
}

void MultiView::configure(QSharedPointer<Settings> settings, QString val, QString key)
{
	OC_METHODGATE();
	k = key.trimmed();
	if ("" != k && settings != nullptr) {
		val = settings->getCustomSetting(k, val);
	}
	QAbstractButton *but = nullptr;
	if (GRID_KEY == val) {
		but = ui->pushButtonViewGrid;
	} else if (LIST_KEY == val) {
		but = ui->pushButtonViewList;
	} else if (DETAILS_KEY == val) {
		but = ui->pushButtonViewDetails;
	} else if (TREE_KEY == val) {
		but = ui->pushButtonViewTree;
	} else {
		qWarning() << "ERROR: NO SUITABLE BUTTON NAME FOUND FOR " << val;
		return;
	}
	onViewButtonClicked(but);
}

// --- New: Zoom Handling via Ctrl+Scroll ---

void MultiView::wheelEvent(QWheelEvent *event)
{
	OC_METHODGATE();
	if (event->modifiers() & Qt::ControlModifier) {
		int delta = event->angleDelta().y();
		const double step = 0.1;
		double newZoom = m_zoomFactor;
		if (delta > 0){
			newZoom += step;
		}
		else if (delta < 0){
			newZoom -= step;
		}
		setZoomFactor(newZoom);
		event->accept();
		return;
	}
	QWidget::wheelEvent(event);
}

void MultiView::setZoomFactor(double factor)
{
	OC_METHODGATE();
	if (factor < 0.5){
		factor = 0.5;
	}
	else if (factor > 3.0){
		factor = 3.0;
	}
	m_zoomFactor = factor;
	QSize baseIconSize(64, 64);
	if (ui->listView != nullptr){
		ui->listView->setIconSize(QSize(baseIconSize.width() * m_zoomFactor, baseIconSize.height() * m_zoomFactor));
	}
	if (ui->tableView != nullptr){
		ui->tableView->setIconSize(QSize(baseIconSize.width() * m_zoomFactor, baseIconSize.height() * m_zoomFactor));
	}
	if (ui->treeView != nullptr){
		ui->treeView->setIconSize(QSize(baseIconSize.width() * m_zoomFactor, baseIconSize.height() * m_zoomFactor));
	}
}


void MultiView::refreshView(){
	OC_METHODGATE();
	if (ui->listView != nullptr){
		ui->listView->reset();
	}
	if (ui->tableView != nullptr){
		ui->tableView->reset();
	}
	if (ui->treeView != nullptr){
		ui->treeView->reset();
	}
}

void MultiView::filterChanged(const QString &filter){
	OC_METHODGATE();
	mFilter->setFilterFixedString(filter);
}

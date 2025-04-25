#include "LogWidget.hpp"
#include "ui_LogWidget.h"

#include "log/LogModel.hpp"
#include "node/Node.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QFontDatabase>
#include <QSortFilterProxyModel>

LogWidget::LogWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::LogWidget)
	, mConfigureHelper("LogWidget", true, false, false, true, false)
{
	OC_METHODGATE();
	ui->setupUi(this);
	QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
	ui->listViewLogFeed->setFont(fixedFont);
	QPalette p = ui->listViewLogFeed->palette();
	p.setColor(QPalette::Text, QColor(64, 255, 128));
	ui->listViewLogFeed->setPalette(p);
}

LogWidget::~LogWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}

void LogWidget::setStorage(QSharedPointer<LogStorage> storage){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		mModel = QSharedPointer<LogModel>::create(storage);
		delete mProxy;
		mProxy = new QSortFilterProxyModel(this);
		mProxy->setSourceModel(mModel.data());
		mProxy->setDynamicSortFilter(true);
		mProxy->setSortRole(Qt::DisplayRole);
		
		ui->listViewLogFeed->setModel(mProxy);
		connect(ui->lineEditLogFilter, &QLineEdit::textChanged, mModel.data(), &LogModel::setFilter);
		connect(storage.data(), &LogStorage::logChanged, mModel.data(), &LogModel::updateLogs, Qt::QueuedConnection);
		
		// 6) now keep the view scrolled to the correct end:
		auto scrollToEnd = [this]() {
			// queue it so it happens *after* the proxy has updated
			QTimer::singleShot(0, this, [this]() {
				if (mScrollDirection)
					ui->listViewLogFeed->scrollToTop();
				else
					ui->listViewLogFeed->scrollToBottom();
			});
		};
		connect(mProxy, &QAbstractItemModel::modelReset, this, scrollToEnd, Qt::QueuedConnection);
		connect(mProxy, &QAbstractItemModel::rowsInserted, this, scrollToEnd, Qt::QueuedConnection);
		scrollToEnd();
	}
}

void LogWidget::configure(QSharedPointer<LogStorage> storage){
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		setStorage(storage);
	}
}

void LogWidget::configure(QSharedPointer<Node> node){
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		mNode = node;
		if(!mNode.isNull()){
			setStorage(mNode->logStorage());
		}
	}
}

void LogWidget::scrollToEnd(){
	OC_METHODGATE();
	if (mScrollDirection) {
		ui->listViewLogFeed->scrollToTop();
	} else {
		ui->listViewLogFeed->scrollToBottom();
	}
}

void LogWidget::setLogDirection(const bool scrollDirDown){
	OC_METHODGATE();
	mScrollDirection = scrollDirDown;
	if (mProxy) {
		if (mScrollDirection) {
			mProxy->sort(0, Qt::DescendingOrder);
		} else {
			mProxy->sort(0, Qt::AscendingOrder);
		}
	}
	scrollToEnd();
}

void LogWidget::setEnableFilter(const bool showFilter){
	OC_METHODGATE();
	ui->lineEditLogFilter->setVisible(showFilter);
	ui->labelLogFilter->setVisible(showFilter);
}

QSharedPointer<LogModel> LogWidget::model(){
	OC_METHODGATE();
	return mModel;
}

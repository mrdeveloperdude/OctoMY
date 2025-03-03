#include "LogWidget.hpp"
#include "ui_LogWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include "node/Node.hpp"
#include "../../liblog/log/LogModel.hpp"


LogWidget::LogWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::LogWidget)
	, mConfigureHelper("LogWidget", true, false, false, true, false)
	, mScrollDirDown(false)
{
	OC_METHODGATE();
	ui->setupUi(this);
}

LogWidget::~LogWidget()
{
	OC_METHODGATE();
	delete ui;
	ui=nullptr;
}


void LogWidget::configure(QSharedPointer<Node> node){
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		mNode = node;
		if(!mNode.isNull()){
			auto *model = OC_NEW LogModel(mNode->logStorage());
			ui->listViewLogFeed->setModel(model);
			connect(ui->lineEditLogFilter, &QLineEdit::textChanged, model, &LogModel::setFilter);
		}
	}
}



void LogWidget::setLogDirection(const bool scrollDirDown){
	OC_METHODGATE();
	mScrollDirDown = scrollDirDown;
}
//m_view->scrollToBottom(); // Scroll down automatically

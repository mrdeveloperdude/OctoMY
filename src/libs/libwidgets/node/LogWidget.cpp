#include "LogWidget.hpp"
#include "ui_LogWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QScrollBar>

LogWidget::LogWidget(QWidget *parent)
	: QPlainTextEdit(parent)
	, ui(OC_NEW Ui::LogWidget)
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


void LogWidget::setDirection(bool down)
{
	OC_METHODGATE();
	mScrollDirDown=down;
	appendLog("");
}

void LogWidget::appendLog(const QString& text)
{
	OC_METHODGATE();
	qDebug()<<"GOT LOG MESSAGE"<<text;
	QString t=text.trimmed()+"\n";
	if(mScrollDirDown) {
		appendPlainText(t);
		QScrollBar *vsb=verticalScrollBar();
		if(nullptr!=vsb) {
			vsb->setValue(vsb->maximum());
		}
	} else {
		moveCursor(QTextCursor::Start);
		insertPlainText(t);
		QScrollBar *vsb=verticalScrollBar();
		if(nullptr!=vsb) {
			vsb->setValue(vsb->minimum());
		}
	}
}

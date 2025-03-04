#include "LogWidget.hpp"
#include "ui_LogWidget.h"

#include "utility/Standard.hpp"

#include <QScrollBar>

LogWidget::LogWidget(QWidget *parent)
	: QPlainTextEdit(parent)
	, ui(OC_NEW Ui::LogWidget)
	, mScrollDirDown(false)
{
	ui->setupUi(this);

}

LogWidget::~LogWidget()
{
	delete ui;
	ui=nullptr;
}


void LogWidget::setDirection(bool down)
{
	mScrollDirDown=down;
	appendLog("");
}

void LogWidget::appendLog(const QString& text)
{
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

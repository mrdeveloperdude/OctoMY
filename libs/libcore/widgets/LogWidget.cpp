#include "LogWidget.hpp"
#include "ui_LogWidget.h"

#include <QScrollBar>

LogWidget::LogWidget(QWidget *parent) :
	QPlainTextEdit(parent),
	ui(new Ui::LogWidget)
{
	ui->setupUi(this);

}

LogWidget::~LogWidget()
{
	delete ui;
}


void LogWidget::appendLog(const QString& text){
	appendPlainText(text);
	QScrollBar *vsb=verticalScrollBar();
	if(0!=vsb){
		vsb->setValue(vsb->maximum());
	}
}


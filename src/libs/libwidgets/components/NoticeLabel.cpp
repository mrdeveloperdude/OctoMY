#include "NoticeLabel.hpp"
#include "ui_NoticeLabel.h"
#include "uptime/MethodGate.hpp"

static QPalette generateNoticePalette(){
	QPalette palette;
	QBrush brush(QColor(0, 0, 0, 255));
	QBrush brush1(QColor(226, 172, 10, 255));
	QBrush brush2(QColor(255, 215, 95, 255));
	QBrush brush3(QColor(240, 193, 52, 255));
	QBrush brush4(QColor(113, 86, 5, 255));
	QBrush brush5(QColor(151, 115, 6, 255));
	QBrush brush6(QColor(255, 255, 255, 255));
	QBrush brush7(QColor(240, 213, 132, 255));
	QBrush brush8(QColor(255, 255, 220, 255));
	palette.setBrush(QPalette::AlternateBase, brush7);
	palette.setBrush(QPalette::Base, brush6);
	palette.setBrush(QPalette::BrightText, brush6);
	palette.setBrush(QPalette::Button, brush1);
	palette.setBrush(QPalette::ButtonText, brush);
	palette.setBrush(QPalette::Dark, brush4);
	palette.setBrush(QPalette::Light, brush2);
	palette.setBrush(QPalette::Mid, brush5);
	palette.setBrush(QPalette::Midlight, brush3);
	palette.setBrush(QPalette::Shadow, brush);
	palette.setBrush(QPalette::Text, brush);
	palette.setBrush(QPalette::ToolTipBase, brush8);
	palette.setBrush(QPalette::ToolTipText, brush);
	palette.setBrush(QPalette::Window, brush1);
	palette.setBrush(QPalette::WindowText, brush);
	return palette;
}



NoticeLabel::NoticeLabel(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::NoticeLabel)
{
	OC_METHODGATE();
	ui->setupUi(this);
	ui->labelNotice->setTextFormat(Qt::PlainText);
	ui->labelNotice->setAutoFillBackground(true);
	ui->labelNotice->setPalette(generateNoticePalette());
}

NoticeLabel::~NoticeLabel()
{
	OC_METHODGATE();
	delete ui;
}


void NoticeLabel::notice(const QString &message){
	OC_METHODGATE();
	setVisible(true);
	ui->labelNotice->setText(message);
	
}

void NoticeLabel::setDeleteOnClose(bool deleteOnClose){
	OC_METHODGATE();
	mDeleteOnClose = deleteOnClose;
}

void NoticeLabel::closeNotice(){
	OC_METHODGATE();
	setVisible(false);
	if(mDeleteOnClose){
		deleteLater();
	}
}

#include "NoticeList.hpp"
#include "ui_NoticeList.h"


#include "components/NoticeLabel.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


#include <QBoxLayout>
#include <QScrollBar>

NoticeList::NoticeList(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::NoticeList)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


NoticeList::~NoticeList()
{
	OC_METHODGATE();
	delete ui;
}


void NoticeList::appendNotice(const QString &notice)
{
	OC_METHODGATE();
	auto label = OC_NEW NoticeLabel(this);
	label->notice(notice);
	label->setDeleteOnClose(true);
	auto layout = qobject_cast<QVBoxLayout *>(ui->widgetCompressedContent->layout());
	if (layout) {
		layout->insertWidget(0, label); // Insert the new label at the top
	}
	label->setParent(ui->widgetCompressedContent);
	ui->scrollAreaNotices->verticalScrollBar()->setValue(ui->scrollAreaNotices->verticalScrollBar()->minimum());
}

#include "ServicesDebugWidget.hpp"
#include "ui_ServicesDebugWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

#include "node/Node.hpp"
#include "service/ServiceLevelManager.hpp"
#include "service/ServiceManager.hpp"

ServicesDebugWidget::ServicesDebugWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ServicesDebugWidget)
{
	ui->setupUi(this);
	auto ipt=new QTableWidgetItem();
	ipt->setFlags( (ipt->flags()) & (~(Qt::ItemIsUserCheckable| Qt::ItemIsEditable)) );
	ui->tableWidgetServices->setItemPrototype(ipt);

	ui->tableWidgetServices->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	//ui->tableWidgetServices->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

}

ServicesDebugWidget::~ServicesDebugWidget()
{
	delete ui;
}


void ServicesDebugWidget::configure(QSharedPointer <Node> node)
{
	OC_METHODGATE();
	mNode=node;
	updateServiceTable();
}


QTableWidgetItem *ServicesDebugWidget::tableItem(const bool s)
{
	OC_METHODGATE();
	auto i=ui->tableWidgetServices->itemPrototype()->clone();
	i->setCheckState(s?Qt::Checked:Qt::Unchecked);
	return i;
}


QTableWidgetItem *ServicesDebugWidget::tableItem(const QString s)
{
	OC_METHODGATE();
	auto i=ui->tableWidgetServices->itemPrototype()->clone();
	i->setText(s);
	return i;
}


void ServicesDebugWidget::setServiceTableItem(const int index, const QString serviceName, const bool expected, const bool actual)
{
	OC_METHODGATE();
	ui->tableWidgetServices->setItem(index, 0, tableItem(QString::number(index)));
	ui->tableWidgetServices->setItem(index, 1, tableItem(serviceName));
	ui->tableWidgetServices->setItem(index, 2, tableItem(expected));
	ui->tableWidgetServices->setItem(index, 3, tableItem(actual));
}


void ServicesDebugWidget::updateServiceTable()
{
	OC_METHODGATE();
	if(!mNode.isNull()) {
		ui->tableWidgetServices->setRowCount(1);
		auto slm=mNode->serviceLevelManager();
		if(!slm.isNull()) {
			auto sm=slm->serviceManager();
			if(!sm.isNull()) {
				auto services=sm->all();
				ui->tableWidgetServices->setRowCount(services.count());
				int i=0;
				for(QString service:services) {
					setServiceTableItem(i++, service, sm->activatedWanted(service), sm->activatedActual(service));
				}
			} else {
				ui->tableWidgetServices->setRowCount(1);
				setServiceTableItem(0, "ERROR: NO SM", false, false);
			}
		} else {
			ui->tableWidgetServices->setRowCount(1);
			setServiceTableItem(0, "ERROR: NO SLM", false, false);
		}
	} else {
		ui->tableWidgetServices->setRowCount(1);
		setServiceTableItem(0, "ERROR: NO NODE", false, false);
	}
}

void ServicesDebugWidget::on_pushButtonUpdate_clicked()
{
	OC_METHODGATE();
	updateServiceTable();
}

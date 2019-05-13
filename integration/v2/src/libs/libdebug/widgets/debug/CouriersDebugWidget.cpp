#include "CouriersDebugWidget.hpp"

#include "ui_CouriersDebugWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

#include "utility/time/HumanTime.hpp"
#include "utility/color/Color.hpp"

#include "node/Node.hpp"
#include "service/ServiceLevelManager.hpp"
#include "service/ServiceManager.hpp"

CouriersDebugWidget::CouriersDebugWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::CouriersDebugWidget)
	, mConfigureHelper("CouriersDebugWidget", true, false, false, true, false)
	, mLastUpdate(0)
{
	ui->setupUi(this);
	auto ipt=new QTableWidgetItem();
	ipt->setFlags( (ipt->flags()) & (~(Qt::ItemIsUserCheckable| Qt::ItemIsEditable)) );
	ui->tableWidgetCouriers->setItemPrototype(ipt);

	ui->tableWidgetCouriers->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	//ui->tableWidgetCouriers->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

}

CouriersDebugWidget::~CouriersDebugWidget()
{
	delete ui;
}


void CouriersDebugWidget::configure(QSharedPointer <Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode=node;
		if(!mNode.isNull()) {
			ui->tableWidgetCouriers->setRowCount(1);
			auto slm=mNode->serviceLevelManager();
			if(!slm.isNull()) {
				/*
				if(!connect(slm.data(), &ServiceLevelManager::CouriersChanged, this, &CouriersDebugWidget::onCouriersChanged, OC_CONTYPE )) {
					qWarning()<<"ERROR: Could not connect";
				}
				*/
			}
		}
		if(!connect(ui->checkBoxUpdateRealtime, &QCheckBox::toggled, this, &CouriersDebugWidget::onRealtimeChanged, OC_CONTYPE )) {
			qWarning()<<"ERROR: Could not connect";
		}
		// We sacrifice quality since this is for debugging purpose onle and we want this to have the least impact on the runtime of non-debug code
		mTimer.setTimerType(Qt::VeryCoarseTimer);
		mTimer.setInterval(1000/15);// 15 FPS
		if(!connect(&mTimer, &QTimer::timeout, this, &CouriersDebugWidget::onTimer, OC_CONTYPE )) {
			qWarning()<<"ERROR: Could not connect";
		}
		onRealtimeChanged(ui->checkBoxUpdateRealtime->isChecked());
	}
}


QTableWidgetItem *CouriersDebugWidget::tableItem(const bool s)
{
	OC_METHODGATE();
	auto i=ui->tableWidgetCouriers->itemPrototype()->clone();
	i->setCheckState(s?Qt::Checked:Qt::Unchecked);
	return i;
}


QTableWidgetItem *CouriersDebugWidget::tableItem(const QString s)
{
	OC_METHODGATE();
	auto i=ui->tableWidgetCouriers->itemPrototype()->clone();
	i->setText(s);
	return i;
}

void CouriersDebugWidget::triggerUpdate(){
	OC_METHODGATE();
	mLastUpdate=utility::time::currentMsecsSinceEpoch<quint64>();
	if(ui->checkBoxUpdateRealtime->isChecked()) {
		mTimer.start();
	}
}


void CouriersDebugWidget::setServiceTableItem(const int index, const QString serviceName, const bool expected, const bool actual)
{
	OC_METHODGATE();
	ui->tableWidgetCouriers->setItem(index, 0, tableItem(QString::number(index)));
	ui->tableWidgetCouriers->setItem(index, 1, tableItem(serviceName));
	ui->tableWidgetCouriers->setItem(index, 2, tableItem(expected));
	ui->tableWidgetCouriers->setItem(index, 3, tableItem(actual));
}


void CouriersDebugWidget::updateServiceTable()
{
	OC_METHODGATE();
	QString sig="";
	if(!mNode.isNull()) {
		ui->tableWidgetCouriers->setRowCount(1);
		auto slm=mNode->serviceLevelManager();
		if(!slm.isNull()) {
			auto sm=slm->serviceManager();
			if(!sm.isNull()) {
				auto Couriers=sm->all();
				ui->tableWidgetCouriers->setRowCount(Couriers.count());
				int i=0;
				for(QString service:Couriers) {
					const bool expected=sm->activatedWanted(service);
					const bool actual=sm->activatedActual(service);
					sig+=QString("%1-%2-%3-%4").arg(i).arg(service).arg(expected).arg(actual);
					setServiceTableItem(i++, service, expected, actual);
				}
			} else {
				ui->tableWidgetCouriers->setRowCount(1);
				setServiceTableItem(0, "ERROR: NO SM", false, false);
			}
		} else {
			ui->tableWidgetCouriers->setRowCount(1);
			setServiceTableItem(0, "ERROR: NO SLM", false, false);
		}
	} else {
		ui->tableWidgetCouriers->setRowCount(1);
		setServiceTableItem(0, "ERROR: NO NODE", false, false);
	}
	if(sig!=mLastSig){
		mLastSig=sig;
		triggerUpdate();
	}
}

void CouriersDebugWidget::on_pushButtonUpdate_clicked()
{
	OC_METHODGATE();
	updateServiceTable();
	onTimer();
}

void CouriersDebugWidget::onCouriersChanged()
{
	OC_METHODGATE();
	qDebug()<<"Couriers CHANGED";
	updateServiceTable();
}


void CouriersDebugWidget::onRealtimeChanged(bool realtime)
{
	OC_METHODGATE();
	Q_UNUSED(realtime);
	if(realtime) {
		triggerUpdate();
	} else {
		mTimer.stop();
	}
}


void CouriersDebugWidget::onTimer()
{
	OC_METHODGATE();
	const quint64 blinkSize=1000;
	auto p=palette();
	auto o=p.color(QPalette::Background);
	const auto now=utility::time::currentMsecsSinceEpoch<quint64>();
	const quint64 time=qBound<quint64>(0, (now-mLastUpdate), blinkSize);
	const qreal raw=static_cast<qreal>(blinkSize-time)/ static_cast<qreal>(blinkSize);
	const qreal f=qBound(0.0, raw, 1.0);
	if(f>=0.0) {
		const QColor c=utility::color::mix(o, Qt::red, f);
		//qDebug()<<"COLOR: "<<c<< ", f="<<f<< ", raw="<<raw<< ", time="<<time;
		p.setColor(QPalette::Background, c);
		ui->tableWidgetCouriers->setPalette(p);
	}
}

#include "CouriersDebugWidget.hpp"

#include "ui_CouriersDebugWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

#include "utility/time/HumanTime.hpp"
#include "utility/color/Color.hpp"

#include "node/Node.hpp"

#include "comms/CommsChannel.hpp"
#include "comms/couriers/Courier.hpp"

CouriersDebugWidget::CouriersDebugWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::CouriersDebugWidget)
	, mConfigureHelper("CouriersDebugWidget", true, false, false, true, false)
	, mLastUpdate(0)
{
	OC_METHODGATE();
	ui->setupUi(this);
	auto ipt=new QTableWidgetItem();
	ipt->setFlags( (ipt->flags()) & (~(Qt::ItemIsUserCheckable| Qt::ItemIsEditable)) );
	ui->tableWidgetCouriers->setItemPrototype(ipt);
	ui->tableWidgetCouriers->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

CouriersDebugWidget::~CouriersDebugWidget()
{
	OC_METHODGATE();
	delete ui;
}


void CouriersDebugWidget::configure(QSharedPointer <Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode=node;
		if(!mNode.isNull()) {
			ui->tableWidgetCouriers->setRowCount(1);
			auto cc=mNode->comms();
			if(!cc.isNull()) {
				auto couriers=cc->couriers();

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
		mTimer.setInterval(1000);// 1 FPS
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

void CouriersDebugWidget::triggerUpdate()
{
	OC_METHODGATE();
	mLastUpdate=utility::time::currentMsecsSinceEpoch<quint64>();
	if(ui->checkBoxUpdateRealtime->isChecked()) {
		// mTimer.start();
	}
}

QString	CouriersDebugWidget::setCourierTableItem(const int index, const QSharedPointer<Courier> courier)
{
	OC_METHODGATE();
	QString ret;
	if(!courier.isNull()) {
		const auto name=courier->name();
		const auto dest=courier->destination();
		const auto id=courier->id();
		const auto serial=courier->serial();
		const auto lastOpportunity=courier->lastOpportunity();
		const auto mandate=courier->mandate();
		ret=QString("%1-%2-%3-%4-%5-%6-%7").arg(index).arg(name).arg(dest).arg(id).arg(serial).arg(lastOpportunity).arg(mandate.toString());
		ui->tableWidgetCouriers->setItem(index, 0, tableItem(QString::number(index)));
		ui->tableWidgetCouriers->setItem(index, 1, tableItem(name));
		ui->tableWidgetCouriers->setItem(index, 2, tableItem(dest));
		ui->tableWidgetCouriers->setItem(index, 3, tableItem(id));
		ui->tableWidgetCouriers->setItem(index, 4, tableItem(QString::number(serial)));
		ui->tableWidgetCouriers->setItem(index, 5, tableItem(lastOpportunity));
		ui->tableWidgetCouriers->setItem(index, 6, tableItem(mandate.toString()));
	} else {
		ui->tableWidgetCouriers->setItem(index, 0, tableItem(QString::number(index)));
		ret="NO Courier";
		ui->tableWidgetCouriers->setItem(index, 1, tableItem(ret));
	}
	return ret;
}


QString CouriersDebugWidget::setCourierTableItem(const int index, const QString string)
{
	OC_METHODGATE();
	ui->tableWidgetCouriers->setItem(index, 0, tableItem(QString::number(index)));
	ui->tableWidgetCouriers->setItem(index, 1, tableItem(string));
	return string;
}


void CouriersDebugWidget::updateServiceTable()
{
	OC_METHODGATE();
	QString sig="";
	ui->tableWidgetCouriers->setRowCount(1);
	if(!mNode.isNull()) {
		auto cc=mNode->comms();
		if(!cc.isNull()) {
			auto couriers=cc->couriers();
			auto ct=couriers.count();
			if(ct>0) {
				ui->tableWidgetCouriers->setRowCount(ct);
				int i=0;
				for(auto courier:couriers) {
					sig+=setCourierTableItem(i++, courier);
				}
			} else {
				sig+=setCourierTableItem(0, "No couriers");
			}
		} else {
			sig+=setCourierTableItem(0, "ERROR: NO CC");
		}
	} else {
		sig+=setCourierTableItem(0, "ERROR: NO NODE");
	}
	if(sig!=mLastSig) {
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

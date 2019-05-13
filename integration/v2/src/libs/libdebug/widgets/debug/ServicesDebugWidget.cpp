#include "ServicesDebugWidget.hpp"
#include "ui_ServicesDebugWidget.h"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

#include "utility/time/HumanTime.hpp"
#include "utility/color/Color.hpp"

#include "node/Node.hpp"
#include "service/ServiceLevelManager.hpp"
#include "service/ServiceManager.hpp"

ServicesDebugWidget::ServicesDebugWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::ServicesDebugWidget)
	, mConfigureHelper("ServicesDebugWidget", true, false, false, true, false)
	, mLastUpdate(0)
{
	OC_METHODGATE();
	ui->setupUi(this);
	auto ipt=new QTableWidgetItem();
	ipt->setFlags( (ipt->flags()) & (~(Qt::ItemIsUserCheckable| Qt::ItemIsEditable)) );
	ui->tableWidgetServices->setItemPrototype(ipt);

	ui->tableWidgetServices->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	//ui->tableWidgetServices->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

}

ServicesDebugWidget::~ServicesDebugWidget()
{
	OC_METHODGATE();
	delete ui;
}


void ServicesDebugWidget::configure(QSharedPointer <Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode=node;
		if(!mNode.isNull()) {
			ui->tableWidgetServices->setRowCount(1);
			auto slm=mNode->serviceLevelManager();
			if(!slm.isNull()) {
				if(!connect(slm.data(), &ServiceLevelManager::servicesChanged, this, &ServicesDebugWidget::onServicesChanged, OC_CONTYPE )) {
					qWarning()<<"ERROR: Could not connect";
				}
			}
		}
		if(!connect(ui->checkBoxUpdateRealtime, &QCheckBox::toggled, this, &ServicesDebugWidget::onRealtimeChanged, OC_CONTYPE )) {
			qWarning()<<"ERROR: Could not connect";
		}
		// We sacrifice quality since this is for debugging purpose onle and we want this to have the least impact on the runtime of non-debug code
		mTimer.setTimerType(Qt::VeryCoarseTimer);
		mTimer.setInterval(1000/15);// 15 FPS
		if(!connect(&mTimer, &QTimer::timeout, this, &ServicesDebugWidget::onTimer, OC_CONTYPE )) {
			qWarning()<<"ERROR: Could not connect";
		}
		onRealtimeChanged(ui->checkBoxUpdateRealtime->isChecked());
	}
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

void ServicesDebugWidget::triggerUpdate()
{
	OC_METHODGATE();
	mLastUpdate=utility::time::currentMsecsSinceEpoch<quint64>();
	if(ui->checkBoxUpdateRealtime->isChecked()) {
		mTimer.start();
	}
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
	QString sig="";
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
					const bool expected=sm->activatedWanted(service);
					const bool actual=sm->activatedActual(service);
					sig+=QString("%1-%2-%3-%4").arg(i).arg(service).arg(expected).arg(actual);
					setServiceTableItem(i++, service, expected, actual);
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
	if(sig!=mLastSig) {
		mLastSig=sig;
		triggerUpdate();
	}
}

void ServicesDebugWidget::on_pushButtonUpdate_clicked()
{
	OC_METHODGATE();
	updateServiceTable();
	onTimer();
}

void ServicesDebugWidget::onServicesChanged()
{
	OC_METHODGATE();
	qDebug()<<"SERVICES CHANGED";
	updateServiceTable();
}


void ServicesDebugWidget::onRealtimeChanged(bool realtime)
{
	OC_METHODGATE();
	Q_UNUSED(realtime);
	if(realtime) {
		triggerUpdate();
	} else {
		mTimer.stop();
	}
}


void ServicesDebugWidget::onTimer()
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
		ui->tableWidgetServices->setPalette(p);
	}
}


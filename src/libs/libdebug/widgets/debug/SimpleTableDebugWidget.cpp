#include "SimpleTableDebugWidget.hpp"
#include "ui_SimpleTableDebugWidget.h"

#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "utility/time/HumanTime.hpp"
#include "utility/color/Color.hpp"


#include <QDebug>
#include <QtGlobal>

SimpleTableDebugWidget::SimpleTableDebugWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::SimpleTableDebugWidget)
	, mConfigureHelper("SimpleTableDebugWidget", true, false, false, true, false)
	, mBlinkStartTime(0)
	, mUiUpdateInterval(0)
	, mDataUpdateInterval(0)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


SimpleTableDebugWidget::~SimpleTableDebugWidget()
{
	OC_METHODGATE();
	delete ui;
}



void SimpleTableDebugWidget::configure(QString name, int uiUpdateInterval, int dataUpdateInterval)
{
	OC_METHODGATE();
	Q_UNUSED(name);
	if(mConfigureHelper.configure()) {
		mUiUpdateInterval=uiUpdateInterval;
		mDataUpdateInterval=dataUpdateInterval;
		if(!connect(ui->pushButtonUpdateRealtime, &QPushButton::toggled, this, &SimpleTableDebugWidget::onRealtimeChangedWrapper, OC_CONTYPE )) {
			qWarning()<<"ERROR: Could not connect ";
		}
		// We sacrifice quality since this is for debugging purpose onle and we want this to have the least impact on the runtime of non-debug code
		mTimerUI.setTimerType(Qt::PreciseTimer);
		mTimerUI.setInterval(mUiUpdateInterval);
		mTimerUI.setSingleShot(false);
		if(!connect(&mTimerUI, &QTimer::timeout, this, &SimpleTableDebugWidget::onUITimerWrapper, OC_CONTYPE )) {
			qWarning()<<"ERROR: Could not connect ";
		}
		mTimerData.setTimerType(Qt::VeryCoarseTimer);
		mTimerData.setInterval(mDataUpdateInterval);
		mTimerData.setSingleShot(true);
		if(!connect(&mTimerData, &QTimer::timeout, this, &SimpleTableDebugWidget::onDataTimerWrapper, OC_CONTYPE )) {
			qWarning()<<"ERROR: Could not connect ";
		}
		onRealtimeChangedWrapper(ui->pushButtonUpdateRealtime->isChecked());
		ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	}
}


static QString mapToString(QMap<QString, QString> map)
{
	QString out;
	for(auto it=map.constBegin(), end=map.constEnd(); it!=end; ++it) {
		out+=it.key()+"="+it.value()+"\n";
	}
	return out;
}


void SimpleTableDebugWidget::setData(QMap<QString, QString> data)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		QString signature=mapToString(data);
		if(signature!=mLastDataSignature) {
			mLastDataSignature=signature;
			ui->tableView->setData(data);
			triggerUIBlink();
		}
	}
}


void SimpleTableDebugWidget::triggerUIBlink()
{
	OC_METHODGATE();
	mBlinkStartTime=utility::time::currentMsecsSinceEpoch<quint64>();
	mTimerUI.start();
}


void SimpleTableDebugWidget::onRealtimeChangedWrapper(bool realtime)
{
	OC_METHODGATE();
	onRealtimeChanged(realtime);
	onDataTimerWrapper();
}


void SimpleTableDebugWidget::onDataTimerWrapper()
{
	OC_METHODGATE();
	//qDebug()<<"DATA";
	onTimer();
	if(ui->pushButtonUpdateRealtime->isChecked()) {
		mTimerData.start();
	}
}


void SimpleTableDebugWidget::onUITimerWrapper()
{
	OC_METHODGATE();
	const quint64 blinkSize=1000;
	auto p=palette();
	auto o=p.color(QPalette::Background);
	const auto now=utility::time::currentMsecsSinceEpoch<quint64>();
	const quint64 time=qBound<quint64>(0, (now-mBlinkStartTime), blinkSize);
	const qreal raw=static_cast<qreal>(blinkSize-time)/ static_cast<qreal>(blinkSize);
	const qreal f=qBound(0.0, raw, 1.0);
	//qDebug()<<"UI "<<f;
	if(f>0.0) {
		const QColor c=utility::color::mix(o, Qt::red, f);
		//qDebug()<<"COLOR: "<<c<< ", f="<<f<< ", raw="<<raw<< ", time="<<time;
		p.setColor(QPalette::Background, c);
		ui->tableView->setPalette(p);
	} else {
		mTimerUI.stop();
	}
}


void SimpleTableDebugWidget::onRealtimeChanged(bool realtime)
{
	OC_METHODGATE();
	Q_UNUSED(realtime);
	// This is a no-op placeholder that will be overriden to be useful
}


void SimpleTableDebugWidget::onTimer()
{
	OC_METHODGATE();
	// This is a no-op placeholder that will be overriden to be useful
}

void SimpleTableDebugWidget::on_pushButtonUpdate_clicked()
{
	OC_METHODGATE();
	onDataTimerWrapper();
}

void SimpleTableDebugWidget::on_pushButtonSave_clicked()
{
	OC_METHODGATE();
	emit saveButtonPressed();
}

void SimpleTableDebugWidget::on_pushButtonLoad_clicked()
{
	OC_METHODGATE();
	emit loadButtonPressed();
}

void SimpleTableDebugWidget::on_pushButtonSync_clicked()
{
	OC_METHODGATE();
	syncButtonPressed();
}

void SimpleTableDebugWidget::on_pushButtonClear_clicked()
{
	OC_METHODGATE();
	clearButtonPressed();
}

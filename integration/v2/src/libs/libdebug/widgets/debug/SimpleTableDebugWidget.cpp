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
	, mLastUpdate(0)
{
	OC_METHODGATE();
	ui->setupUi(this);
}


SimpleTableDebugWidget::~SimpleTableDebugWidget()
{
	OC_METHODGATE();
	delete ui;
}



void SimpleTableDebugWidget::configure(QString name)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		ui->labelName->setText(name);
		if(!connect(ui->checkBoxUpdateRealtime, &QCheckBox::toggled, this, &SimpleTableDebugWidget::onRealtimeChangedWrapper, OC_CONTYPE )) {
			qWarning()<<"ERROR: Could not connect ";
		}
		// We sacrifice quality since this is for debugging purpose onle and we want this to have the least impact on the runtime of non-debug code
		mTimer.setTimerType(Qt::VeryCoarseTimer);
		mTimer.setInterval(1000/15);// 15 FPS
		if(!connect(&mTimer, &QTimer::timeout, this, &SimpleTableDebugWidget::onTimerWrapper, OC_CONTYPE )) {
			qWarning()<<"ERROR: Could not connect ";
		}
		onRealtimeChangedWrapper(ui->checkBoxUpdateRealtime->isChecked());
	}
}


void SimpleTableDebugWidget::setData(QMap<QString, QString> data)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		ui->widgetSimpleTable->setData(data);
		mLastUpdate=utility::time::currentMsecsSinceEpoch<quint64>();
		mTimer.start();
	}
}


void SimpleTableDebugWidget::onRealtimeChangedWrapper(bool realtime)
{
	OC_METHODGATE();
	Q_UNUSED(realtime);
	if(realtime) {
		mLastUpdate=utility::time::currentMsecsSinceEpoch<quint64>();
		mTimer.start();
	} else {
		mTimer.stop();
	}
	onRealtimeChanged(realtime);
}


void SimpleTableDebugWidget::onTimerWrapper()
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
	}
	ui->widgetSimpleTable->setPalette(p);
	onTimer();
}


void SimpleTableDebugWidget::onRealtimeChanged(bool realtime)
{
	OC_METHODGATE();
	Q_UNUSED(realtime);
}


void SimpleTableDebugWidget::onTimer()
{
	OC_METHODGATE();
}

void SimpleTableDebugWidget::on_pushButtonUpdate_clicked()
{
	OC_METHODGATE();
	onTimer();
}

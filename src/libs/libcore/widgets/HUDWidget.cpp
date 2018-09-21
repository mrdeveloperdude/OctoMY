#include "HUDWidget.hpp"
#include "ui_HUDWidget.h"

#include "utility/Standard.hpp"
#include "utility/Utility.hpp"
#include <QDateTime>

HUDWidget::HUDWidget(QWidget *parent)
	: QWidget(parent)
	, ui(OC_NEW Ui::HUDWidget)
	, startTime(utility::currentMsecsSinceEpoch<quint64>())
{
	ui->setupUi(this);
	gaugeTimer.setInterval(1000/60);
	gaugeTimer.setTimerType(Qt::PreciseTimer);
	if(!connect(&gaugeTimer,SIGNAL(timeout()),this,SLOT(onGaugeTimer()),OC_CONTYPE)){
		qWarning()<<"ERROR: Could not connect";
	}
	ui->tabWidgetAviation->setCurrentIndex(0);
}

HUDWidget::~HUDWidget()
{
	delete ui;
	ui=nullptr;
}


void HUDWidget::showEvent(QShowEvent *){
	gaugeTimer.start();
}

void HUDWidget::hideEvent(QHideEvent *){
	gaugeTimer.stop();
}


void HUDWidget::onGaugeTimer(){
	const double cmsse=utility::currentMsecsSinceEpoch<quint64>()-startTime;
	const double ms = cmsse/1000.0;

	float alpha     =  0.0f;
	float beta      =  0.0f;
	float roll      =  0.0f;
	float pitch     =  0.0f;
	float heading   =  0.0f;
	float slipSkid  =  0.0f;
	float turnRate  =  0.0f;
	float devH      =  0.0f;
	float devV      =  0.0f;
	float airspeed  =  0.0f;
	float altitude  =  0.0f;
	float pressure  = 28.0f;
	float climbRate =  0.0f;
	float machNo    =  0.0f;
	float adf       =  0.0f;
	float dme       =  0.0f;


	alpha     = static_cast<float>(   20.0 * sin( ms /  10.0 ));
	beta      = static_cast<float>(   15.0 * sin( ms /  10.0 ));
	roll      = static_cast<float>(  180.0 * sin( ms /  10.0 ));
	pitch     = static_cast<float>(   90.0 * sin( ms /  20.0 ));
	heading   = static_cast<float>(  360.0 * sin( ms /  40.0 ));
	slipSkid  = static_cast<float>(    1.0 * sin( ms /  10.0 ));
	turnRate  = static_cast<float>(    7.0 * sin( ms /  10.0 ));
	devH      = static_cast<float>(    1.0 * sin( ms /  20.0 ));
	devV      = static_cast<float>(    1.0 * sin( ms /  20.0 ));
	airspeed  = static_cast<float>(  125.0 * sin( ms /  40.0 ) +  125.0);
	altitude  = static_cast<float>( 9000.0 * sin( ms /  40.0 ) + 9000.0);
	pressure  = static_cast<float>(    2.0 * sin( ms /  20.0 ) +   30.0);
	climbRate = static_cast<float>(  650.0 * sin( ms /  20.0 ));
	machNo    = static_cast<float>( airspeed / 650.0f);
	adf       = static_cast<float>( -360.0 * sin( ms /  50.0 ));
	dme       = static_cast<float>(   99.0 * sin( ms / 100.0 ));

	ui->widgetPFD->setFlightPathMarker ( alpha, beta );
	ui->widgetPFD->setRoll          ( roll     );
	ui->widgetPFD->setPitch         ( pitch     );
	ui->widgetPFD->setSlipSkid      ( slipSkid  );
	ui->widgetPFD->setTurnRate      ( turnRate / 6.0f );
	ui->widgetPFD->setDevH          ( devH      );
	ui->widgetPFD->setDevV          ( devV      );
	ui->widgetPFD->setHeading       ( heading   );
	ui->widgetPFD->setAirspeed      ( airspeed  );
	ui->widgetPFD->setMachNo        ( machNo    );
	ui->widgetPFD->setAltitude      ( altitude  );
	ui->widgetPFD->setPressure      ( pressure  );
	ui->widgetPFD->setClimbRate     ( climbRate / 100.0f );

	ui->widgetNAV->setHeading    ( heading   );
	ui->widgetNAV->setHeadingBug ( 0.0f );
	ui->widgetNAV->setCourse     ( 0.0f );
	ui->widgetNAV->setBearing    ( adf  , true );
	ui->widgetNAV->setDeviation  ( devH , true );
	ui->widgetNAV->setDistance   ( dme  , true );

	ui->widgetSix->setRoll      ( roll      );
	ui->widgetSix->setPitch     ( pitch     );
	ui->widgetSix->setAltitude  ( altitude  );
	ui->widgetSix->setPressure  ( pressure  );
	ui->widgetSix->setAirspeed  ( airspeed  );
	ui->widgetSix->setHeading   ( heading   );
	ui->widgetSix->setSlipSkid  ( slipSkid * 15.0f );
	ui->widgetSix->setTurnRate  ( turnRate  );
	ui->widgetSix->setClimbRate ( climbRate );
	if(ui->tabWidgetAviation->currentWidget()==ui->tabPFD){
		ui->widgetPFD->update();
	}
	if(ui->tabWidgetAviation->currentWidget()==ui->tabNAV){
		ui->widgetNAV->update();
	}
	if(ui->tabWidgetAviation->currentWidget()==ui->tabSix){
		ui->widgetSix->update();
	}
}

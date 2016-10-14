#include "HUDWidget.hpp"
#include "ui_HUDWidget.h"

#include "../libutil/utility/Standard.hpp"
#include <QDateTime>

HUDWidget::HUDWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::HUDWidget)
	, startTime(QDateTime::currentMSecsSinceEpoch())
{
	ui->setupUi(this);
	gaugeTimer.setInterval(1000/60);
	gaugeTimer.setTimerType(Qt::PreciseTimer);
	if(!connect(&gaugeTimer,SIGNAL(timeout()),this,SLOT(onGaugeTimer()),OC_CONTYPE)){
		qDebug()<<"could not connect";
	}
	ui->tabWidgetAviation->setCurrentIndex(0);
}

HUDWidget::~HUDWidget()
{
	delete ui;
}




void HUDWidget::showEvent(QShowEvent *){
	gaugeTimer.start();
}

void HUDWidget::hideEvent(QHideEvent *){
	gaugeTimer.stop();
}


void HUDWidget::onGaugeTimer(){
	const float cmsse=QDateTime::currentMSecsSinceEpoch()-startTime;
	const float ms = cmsse/1000.0f;

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


	alpha     =   20.0f * sin( ms /  10.0f );
	beta      =   15.0f * sin( ms /  10.0f );
	roll      =  180.0f * sin( ms /  10.0f );
	pitch     =   90.0f * sin( ms /  20.0f );
	heading   =  360.0f * sin( ms /  40.0f );
	slipSkid  =    1.0f * sin( ms /  10.0f );
	turnRate  =    7.0f * sin( ms /  10.0f );
	devH      =    1.0f * sin( ms /  20.0f );
	devV      =    1.0f * sin( ms /  20.0f );
	airspeed  =  125.0f * sin( ms /  40.0f ) +  125.0f;
	altitude  = 9000.0f * sin( ms /  40.0f ) + 9000.0f;
	pressure  =    2.0f * sin( ms /  20.0f ) +   30.0f;
	climbRate =  650.0f * sin( ms /  20.0f );
	machNo    = airspeed / 650.0f;
	adf       = -360.0f * sin( ms /  50.0f );
	dme       =   99.0f * sin( ms / 100.0f );

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

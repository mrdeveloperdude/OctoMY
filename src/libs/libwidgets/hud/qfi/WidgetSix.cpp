#ifndef WIDGETSIX_CPP
#define WIDGETSIX_CPP
#endif

#include "WidgetSix.hpp"
#include "ui_WidgetSix.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


WidgetSix::WidgetSix( QWidget *parent )
	: QWidget ( parent )
	, m_ui ( OC_NEW Ui::WidgetSix )
	, m_widgetADI ( nullptr )
	, m_widgetALT ( nullptr )
	, m_widgetASI ( nullptr )
	, m_widgetHSI ( nullptr )
	, m_widgetTC  ( nullptr )
	, m_widgetVSI ( nullptr )
{
	OC_METHODGATE();
	Q_INIT_RESOURCE(qfi);
	m_ui->setupUi( this );

	m_widgetADI = m_ui->widgetADI;
	m_widgetALT = m_ui->widgetALT;
	m_widgetASI = m_ui->widgetASI;
	m_widgetHSI = m_ui->widgetHSI;
	m_widgetTC  = m_ui->widgetTC;
	m_widgetVSI = m_ui->widgetVSI;
}


WidgetSix::~WidgetSix()
{
	OC_METHODGATE();
	if ( nullptr != m_ui ) {
		delete m_ui;
	}
	m_ui = nullptr;
}


void WidgetSix::update()
{
	OC_METHODGATE();
	m_ui->widgetADI->update();
	m_ui->widgetALT->update();
	m_ui->widgetASI->update();
	m_ui->widgetHSI->update();
	m_ui->widgetTC->update();
	m_ui->widgetVSI->update();
}


void WidgetSix::setRoll( float roll )
{
	OC_METHODGATE();
	m_widgetADI->setRoll( roll );
}


void WidgetSix::setPitch( float pitch )
{
	OC_METHODGATE();
	m_widgetADI->setPitch( pitch );
}


void WidgetSix::setAltitude( float altitude )
{
	OC_METHODGATE();
	m_widgetALT->setAltitude( altitude );
}


void WidgetSix::setPressure( float pressure )
{
	OC_METHODGATE();
	m_widgetALT->setPressure( pressure );
}


void WidgetSix::setAirspeed( float airspeed )
{
	OC_METHODGATE();
	m_widgetASI->setAirspeed( airspeed );
}


void WidgetSix::setHeading( float heading )
{
	OC_METHODGATE();
	m_widgetHSI->setHeading( heading );
}


void WidgetSix::setTurnRate( float turnRate )
{
	OC_METHODGATE();
	m_widgetTC->setTurnRate( turnRate );
}


void WidgetSix::setSlipSkid( float slipSkid )
{
	OC_METHODGATE();
	m_widgetTC->setSlipSkid( slipSkid );
}


void WidgetSix::setClimbRate( float climbRate )
{
	OC_METHODGATE();
	m_widgetVSI->setClimbRate( climbRate );
}


void WidgetSix::setTheme(QString theme)
{
	OC_METHODGATE();
	m_widgetADI ->setTheme(theme);
	m_widgetALT ->setTheme(theme);
	m_widgetASI ->setTheme(theme);
	m_widgetHSI ->setTheme(theme);
	m_widgetTC  ->setTheme(theme);
	m_widgetVSI ->setTheme(theme);
}


QString WidgetSix::theme()
{
	OC_METHODGATE();
	//NOTE: awsumes all have same theme
	return m_widgetVSI->theme();
}

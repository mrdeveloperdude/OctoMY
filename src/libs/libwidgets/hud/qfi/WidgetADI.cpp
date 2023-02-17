#ifndef WIDGETADI_CPP
#define WIDGETADI_CPP
#endif


#include "WidgetADI.hpp"
#include "ui_WidgetADI.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


WidgetADI::WidgetADI( QWidget *parent )
	: QWidget( parent )
	, m_ui( OC_NEW Ui::WidgetADI )
	, m_adi ( nullptr )
	, m_layoutSq ( nullptr )
{
	OC_METHODGATE();
	Q_INIT_RESOURCE(qfi);
	m_ui->setupUi( this );

	setupUi();

	m_adi = m_ui->graphicsADI;
}


WidgetADI::~WidgetADI()
{
	OC_METHODGATE();
	if ( m_layoutSq ) {
		delete m_layoutSq;
	}
	m_layoutSq = nullptr;

	if ( m_ui ) {
		delete m_ui;
	}
	m_ui = nullptr;
}


void WidgetADI::setupUi()
{
	OC_METHODGATE();
	m_layoutSq = OC_NEW LayoutSquare( this );

	m_layoutSq->setContentsMargins( 0, 0, 0, 0 );
	m_layoutSq->addWidget( m_ui->graphicsADI );

	setLayout( m_layoutSq );
}


void WidgetADI::update()
{
	OC_METHODGATE();
	m_adi->update();
}


void WidgetADI::setRoll( float roll )
{
	OC_METHODGATE();
	m_adi->setRoll( roll );
}


void WidgetADI::setPitch( float pitch )
{
	OC_METHODGATE();
	m_adi->setPitch( pitch );
}


void WidgetADI::setTheme(QString theme)
{
	OC_METHODGATE();
	m_adi->setTheme(theme);
}


QString WidgetADI::theme()
{
	OC_METHODGATE();
	return m_adi->theme();
}


#ifndef WIDGETTC_CPP
#define WIDGETTC_CPP
#endif


#include "WidgetTC.hpp"
#include "ui_WidgetTC.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


WidgetTC::WidgetTC( QWidget *parent )
	: QWidget( parent )
	, m_ui( OC_NEW Ui::WidgetTC )
	, m_tc ( nullptr )
	, m_layoutSq ( nullptr )
{
	OC_METHODGATE();
	Q_INIT_RESOURCE(qfi);
	m_ui->setupUi( this );

	setupUi();

	m_tc = m_ui->graphicsTC;
}


WidgetTC::~WidgetTC()
{
	OC_METHODGATE();
	if ( nullptr != m_layoutSq ) {
		delete m_layoutSq;
	}
	m_layoutSq = nullptr;

	if ( nullptr !=  m_ui ) {
		delete m_ui;
	}
	m_ui = nullptr;
}


void WidgetTC::setupUi()
{
	OC_METHODGATE();
	m_layoutSq = OC_NEW LayoutSquare( this );

	m_layoutSq->setContentsMargins( 0, 0, 0, 0 );
	m_layoutSq->addWidget( m_ui->graphicsTC );

	setLayout( m_layoutSq );
}


void WidgetTC::update()
{
	OC_METHODGATE();
	m_tc->update();
}


void WidgetTC::setTurnRate( float turnRate )
{
	OC_METHODGATE();
	m_tc->setTurnRate( turnRate );
}


void WidgetTC::setSlipSkid( float slipSkid )
{
	OC_METHODGATE();
	m_tc->setSlipSkid( slipSkid );
}


void WidgetTC::setTheme(QString theme)
{
	OC_METHODGATE();
	m_tc->setTheme(theme);
}


QString WidgetTC::theme()
{
	OC_METHODGATE();
	return m_tc->theme();
}

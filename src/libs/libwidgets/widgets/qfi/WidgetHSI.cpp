#ifndef WIDGETHSI_CPP
#define WIDGETHSI_CPP
#endif


#include "WidgetHSI.hpp"
#include "ui_WidgetHSI.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


WidgetHSI::WidgetHSI( QWidget *parent )
	: QWidget( parent )
	, m_ui( OC_NEW Ui::WidgetHSI )
	, m_hsi ( nullptr )
	, m_layoutSq ( nullptr )
{
	OC_METHODGATE();
	Q_INIT_RESOURCE(qfi);
	m_ui->setupUi( this );
	setupUi();
	m_hsi = m_ui->graphicsHSI;
}


WidgetHSI::~WidgetHSI()
{
	OC_METHODGATE();
	if ( nullptr!= m_layoutSq ) {
		delete m_layoutSq;
	}
	m_layoutSq = nullptr;

	if ( nullptr!= m_ui ) {
		delete m_ui;
	}
	m_ui = nullptr;
}


void WidgetHSI::setupUi()
{
	OC_METHODGATE();
	m_layoutSq = OC_NEW LayoutSquare( this );
	m_layoutSq->setContentsMargins( 0, 0, 0, 0 );
	m_layoutSq->addWidget( m_ui->graphicsHSI );
	setLayout( m_layoutSq );
}


void WidgetHSI::update()
{
	OC_METHODGATE();
	m_hsi->update();
}


void WidgetHSI::setHeading( float heading )
{
	OC_METHODGATE();
	m_hsi->setHeading( heading );
}


void WidgetHSI::setTheme(QString theme)
{
	OC_METHODGATE();
	m_hsi->setTheme(theme);
}

QString WidgetHSI::theme()
{
	OC_METHODGATE();
	return m_hsi->theme();
}


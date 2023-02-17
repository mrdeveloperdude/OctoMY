#ifndef WIDGETNAV_CPP
#define WIDGETNAV_CPP
#endif

#include "WidgetNAV.hpp"
#include "ui_WidgetNAV.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"



WidgetNAV::WidgetNAV( QWidget *parent )
	: QWidget( parent )
	, m_ui( OC_NEW Ui::WidgetNAV )
	, m_nav ( nullptr )
	, m_layoutSq ( nullptr )
{
	OC_METHODGATE();
	Q_INIT_RESOURCE(qfi);
	m_ui->setupUi( this );

	setupUi();

	m_nav = m_ui->graphicsNAV;
}


WidgetNAV::~WidgetNAV()
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


void WidgetNAV::setupUi()
{
	OC_METHODGATE();
	m_layoutSq = OC_NEW LayoutSquare( this );

	m_layoutSq->setContentsMargins( 0, 0, 0, 0 );
	m_layoutSq->addWidget( m_ui->frameNAV );

	setLayout( m_layoutSq );
}


void WidgetNAV::update()
{
	OC_METHODGATE();
	m_nav->update();
}


void WidgetNAV::setHeading( float heading )
{
	OC_METHODGATE();
	m_nav->setHeading( heading );
}


void WidgetNAV::setHeadingBug( float headingBug )
{
	OC_METHODGATE();
	m_nav->setHeadingBug( headingBug );
}


void WidgetNAV::setCourse( float course )
{
	OC_METHODGATE();
	m_nav->setCourse( course );
}


void WidgetNAV::setBearing( float bearing, bool visible )
{
	OC_METHODGATE();
	m_nav->setBearing( bearing, visible );
}


void WidgetNAV::setDeviation( float deviation, bool visible )
{
	OC_METHODGATE();
	m_nav->setDeviation( deviation, visible );
}


void WidgetNAV::setDistance( float distance, bool visible )
{
	OC_METHODGATE();
	m_nav->setDistance( distance, visible );
}


void WidgetNAV::setTheme(QString theme)
{
	OC_METHODGATE();
	m_nav->setTheme(theme);
}


QString WidgetNAV::theme()
{
	OC_METHODGATE();
	return m_nav->theme();
}

/***************************************************************************//**
 * @file qfi_NAV.cpp
 * @author  Marek M. Cel <marekcel@marekcel.pl>
 *
 * @section LICENSE
 *
 * Copyright (C) 2015 Marek M. Cel
 *
 * This file is part of QFlightInstruments. You can redistribute and modify it
 * under the terms of GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Further information about the GNU General Public License can also be found
 * on the world wide web at http://www.gnu.org.
 *
 * ---
 *
 * Copyright (C) 2015 Marek M. Cel
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 ******************************************************************************/
#ifndef QFI_NAV_CPP
#define QFI_NAV_CPP
#endif

#include "qfi_NAV.h"


#include "uptime/MethodGate.hpp"

#include "uptime/New.hpp"

#include <QtMath>

////////////////////////////////////////////////////////////////////////////////

qfi_NAV::qfi_NAV( QWidget *parent )
	: QGraphicsView ( parent )

	, m_scene ( nullptr )

	, m_itemBack ( nullptr )
	, m_itemMask ( nullptr )
	, m_itemMark ( nullptr )

	, m_itemBrgArrow ( nullptr )
	, m_itemCrsArrow ( nullptr )
	, m_itemDevBar   ( nullptr )
	, m_itemDevScale ( nullptr )
	, m_itemHdgBug   ( nullptr )
	, m_itemHdgScale ( nullptr )

	, m_itemCrsText ( nullptr )
	, m_itemHdgText ( nullptr )
	, m_itemDmeText ( nullptr )

	, m_crsTextColor (   0, 255,   0 )
	, m_hdgTextColor ( 255,   0, 255 )
	, m_dmeTextColor ( 255, 255, 255 )

	, m_heading    ( 0.0f )
	, m_headingBug ( 0.0f )
	, m_course     ( 0.0f )
	, m_bearing    ( 0.0f )
	, m_deviation  ( 0.0f )
	, m_distance   ( 0.0f )

	, m_bearingVisible   ( true )
	, m_deviationVisible ( true )
	, m_distanceVisible  ( true )

	, m_devBarDeltaX_new ( 0.0f )
	, m_devBarDeltaX_old ( 0.0f )
	, m_devBarDeltaY_new ( 0.0f )
	, m_devBarDeltaY_old ( 0.0f )

	, m_scaleX ( 1.0f )
	, m_scaleY ( 1.0f )

	, m_originalPixPerDev ( 52.5f )

	, m_originalNavCtr ( 150.0, 150.0 )

	, m_originalCrsTextCtr (  50.0,  25.0 )
	, m_originalHdgTextCtr ( 250.0,  25.0 )
	, m_originalDmeTextCtr ( 250.0, 275.0 )

	, m_originalHeight ( 300 )
	, m_originalWidth  ( 300 )

	, m_backZ (   0 )
	, m_maskZ ( 100 )
	, m_markZ ( 200 )

	, m_brgArrowZ (  60 )
	, m_crsArrowZ (  70 )
	, m_crsTextZ  ( 130 )
	, m_devBarZ   (  50 )
	, m_devScaleZ (  10 )
	, m_hdgBugZ   ( 120 )
	, m_hdgScaleZ ( 110 )
	, m_hdgTextZ  ( 130 )
	, m_dmeTextZ  ( 130 )
	, m_theme("default")
{
	OC_METHODGATE();
	m_crsTextFont.setFamily( "courier" );
	m_crsTextFont.setPointSizeF( 12.0 );
	m_crsTextFont.setStretch( QFont::Condensed );
	m_crsTextFont.setWeight( QFont::Bold );

	m_hdgTextFont.setFamily( "courier" );
	m_hdgTextFont.setPointSizeF( 12.0 );
	m_hdgTextFont.setStretch( QFont::Condensed );
	m_hdgTextFont.setWeight( QFont::Bold );

	m_dmeTextFont.setFamily( "courier" );
	m_dmeTextFont.setPointSizeF( 10.0 );
	m_dmeTextFont.setStretch( QFont::Condensed );
	m_dmeTextFont.setWeight( QFont::Bold );


	reset();

	m_scene = OC_NEW QGraphicsScene( this );
	setScene( m_scene );

	m_scene->clear();

	init();
}

////////////////////////////////////////////////////////////////////////////////

qfi_NAV::~qfi_NAV()
{
	OC_METHODGATE();
	if ( m_scene ) {
		m_scene->clear();
		delete m_scene;
		m_scene = nullptr;
	}

	reset();
}

////////////////////////////////////////////////////////////////////////////////

void qfi_NAV::reinit()
{
	OC_METHODGATE();
	if ( m_scene ) {
		m_scene->clear();

		init();
	}
}

////////////////////////////////////////////////////////////////////////////////

void qfi_NAV::update()
{
	OC_METHODGATE();
	updateView();

	m_devBarDeltaX_old = m_devBarDeltaX_new;
	m_devBarDeltaY_old = m_devBarDeltaY_new;
}



////////////////////////////////////////////////////////////////////////////////

void qfi_NAV::setTheme(QString theme)
{
	OC_METHODGATE();
	m_theme=theme;
}

////////////////////////////////////////////////////////////////////////////////

QString qfi_NAV::theme()
{
	OC_METHODGATE();
	return m_theme;
}


////////////////////////////////////////////////////////////////////////////////

void qfi_NAV::setHeading( float heading )
{
	OC_METHODGATE();
	m_heading = heading;

	while ( m_heading <   0.0f ) {
		m_heading += 360.0f;
	}
	while ( m_heading > 360.0f ) {
		m_heading -= 360.0f;
	}
}

////////////////////////////////////////////////////////////////////////////////

void qfi_NAV::setHeadingBug( float headingBug )
{
	OC_METHODGATE();
	m_headingBug = headingBug;

	while ( m_headingBug <   0.0f ) {
		m_headingBug += 360.0f;
	}
	while ( m_headingBug > 360.0f ) {
		m_headingBug -= 360.0f;
	}
}

////////////////////////////////////////////////////////////////////////////////

void qfi_NAV::setCourse( float course )
{
	m_course = course;

	while ( m_course <   0.0f ) {
		m_course += 360.0f;
	}
	while ( m_course > 360.0f ) {
		m_course -= 360.0f;
	}
}

////////////////////////////////////////////////////////////////////////////////

void qfi_NAV::setBearing( float bearing, bool visible )
{
	m_bearing        = bearing;
	m_bearingVisible = visible;

	while ( m_bearing <   0.0f ) {
		m_bearing += 360.0f;
	}
	while ( m_bearing > 360.0f ) {
		m_bearing -= 360.0f;
	}
}

////////////////////////////////////////////////////////////////////////////////

void qfi_NAV::setDeviation( float deviation, bool visible )
{
	m_deviation        = deviation;
	m_deviationVisible = visible;

	if ( m_deviation < -1.0f ) {
		m_deviation = -1.0f;
	}
	if ( m_deviation >  1.0f ) {
		m_deviation =  1.0f;
	}
}

////////////////////////////////////////////////////////////////////////////////

void qfi_NAV::setDistance( float distance, bool visible )
{
	OC_METHODGATE();
	m_distance        = qAbs( distance );
	m_distanceVisible = visible;
}

////////////////////////////////////////////////////////////////////////////////

void qfi_NAV::resizeEvent( QResizeEvent *event )
{
	OC_METHODGATE();
	////////////////////////////////////
	QGraphicsView::resizeEvent( event );
	////////////////////////////////////

	reinit();
}

////////////////////////////////////////////////////////////////////////////////

void qfi_NAV::init()
{
	OC_METHODGATE();
	m_scaleX = static_cast<float>(width())  / static_cast<float>(m_originalWidth);
	m_scaleY = static_cast<float>(height()) / static_cast<float>(m_originalHeight);

	m_itemBack = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/nav/nav_back.svg" );
	m_itemBack->setCacheMode( QGraphicsItem::NoCache );
	m_itemBack->setZValue( m_backZ );
	m_itemBack->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ) , true );
	m_scene->addItem( m_itemBack );

	m_itemMask = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/nav/nav_mask.svg" );
	m_itemMask->setCacheMode( QGraphicsItem::NoCache );
	m_itemMask->setZValue( m_maskZ );
	m_itemMask->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ) , true );
	m_scene->addItem( m_itemMask );

	m_itemMark = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/nav/nav_mark.svg" );
	m_itemMark->setCacheMode( QGraphicsItem::NoCache );
	m_itemMark->setZValue( m_markZ );
	m_itemMark->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ) , true );
	m_scene->addItem( m_itemMark );

	m_itemBrgArrow = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/nav/nav_brg_arrow.svg" );
	m_itemBrgArrow->setCacheMode( QGraphicsItem::NoCache );
	m_itemBrgArrow->setZValue( m_brgArrowZ );
	m_itemBrgArrow->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ) , true );
	m_itemBrgArrow->setTransformOriginPoint( m_originalNavCtr );
	m_scene->addItem( m_itemBrgArrow );

	m_itemCrsArrow = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/nav/nav_crs_arrow.svg" );
	m_itemCrsArrow->setCacheMode( QGraphicsItem::NoCache );
	m_itemCrsArrow->setZValue( m_crsArrowZ );
	m_itemCrsArrow->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ) , true );
	m_itemCrsArrow->setTransformOriginPoint( m_originalNavCtr );
	m_scene->addItem( m_itemCrsArrow );

	m_itemDevBar = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/nav/nav_dev_bar.svg" );
	m_itemDevBar->setCacheMode( QGraphicsItem::NoCache );
	m_itemDevBar->setZValue( m_devBarZ );
	m_itemDevBar->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ) , true );
	m_itemDevBar->setTransformOriginPoint( m_originalNavCtr );
	m_scene->addItem( m_itemDevBar );

	m_itemDevScale = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/nav/nav_dev_scale.svg" );
	m_itemDevScale->setCacheMode( QGraphicsItem::NoCache );
	m_itemDevScale->setZValue( m_devScaleZ );
	m_itemDevScale->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ) , true );
	m_itemDevScale->setTransformOriginPoint( m_originalNavCtr );
	m_scene->addItem( m_itemDevScale );

	m_itemHdgBug = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/nav/nav_hdg_bug.svg" );
	m_itemHdgBug->setCacheMode( QGraphicsItem::NoCache );
	m_itemHdgBug->setZValue( m_hdgBugZ );
	m_itemHdgBug->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ) , true );
	m_itemHdgBug->setTransformOriginPoint( m_originalNavCtr );
	m_scene->addItem( m_itemHdgBug );

	m_itemHdgScale = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/nav/nav_hdg_scale.svg" );
	m_itemHdgScale->setCacheMode( QGraphicsItem::NoCache );
	m_itemHdgScale->setZValue( m_hdgScaleZ );
	m_itemHdgScale->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ) , true );
	m_itemHdgScale->setTransformOriginPoint( m_originalNavCtr );
	m_scene->addItem( m_itemHdgScale );

	m_itemCrsText = nullptr;

	m_itemCrsText = OC_NEW QGraphicsTextItem( QString( "CRS 999" ) );
	m_itemCrsText->setCacheMode( QGraphicsItem::NoCache );
	m_itemCrsText->setZValue( m_crsTextZ );
	m_itemCrsText->setDefaultTextColor( m_crsTextColor );
	m_itemCrsText->setFont( m_crsTextFont );
	m_itemCrsText->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ) , true );
	m_itemCrsText->moveBy( static_cast<qreal>(m_scaleX) * ( m_originalCrsTextCtr.x() - m_itemCrsText->boundingRect().width()  / 2.0 ),
						   static_cast<qreal>(m_scaleY) * ( m_originalCrsTextCtr.y() - m_itemCrsText->boundingRect().height() / 2.0 ) );
	m_scene->addItem( m_itemCrsText );

	m_itemHdgText = OC_NEW QGraphicsTextItem( QString( "HDG 999" ) );
	m_itemHdgText->setCacheMode( QGraphicsItem::NoCache );
	m_itemHdgText->setZValue( m_hdgTextZ );
	m_itemHdgText->setDefaultTextColor( m_hdgTextColor );
	m_itemHdgText->setFont( m_hdgTextFont );
	m_itemHdgText->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ) , true );
	m_itemHdgText->moveBy( static_cast<qreal>(m_scaleX) * ( m_originalHdgTextCtr.x() - m_itemHdgText->boundingRect().width()  / 2.0 ),
						   static_cast<qreal>(m_scaleY) * ( m_originalHdgTextCtr.y() - m_itemHdgText->boundingRect().height() / 2.0 ) );
	m_scene->addItem( m_itemHdgText );

	m_itemDmeText = OC_NEW QGraphicsTextItem( QString( "99.9 NM" ) );
	m_itemDmeText->setCacheMode( QGraphicsItem::NoCache );
	m_itemDmeText->setZValue( m_dmeTextZ );
	m_itemDmeText->setDefaultTextColor( m_dmeTextColor );
	m_itemDmeText->setFont( m_dmeTextFont );
	m_itemDmeText->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ) , true );
	m_itemDmeText->moveBy( static_cast<qreal>(m_scaleX) * ( m_originalDmeTextCtr.x() - m_itemDmeText->boundingRect().width()  / 2.0 ),
						   static_cast<qreal>(m_scaleY) * ( m_originalDmeTextCtr.y() - m_itemDmeText->boundingRect().height() / 2.0 ) );
	m_scene->addItem( m_itemDmeText );

	updateView();
}

////////////////////////////////////////////////////////////////////////////////

void qfi_NAV::reset()
{
	OC_METHODGATE();
	m_itemBrgArrow = nullptr;
	m_itemCrsArrow = nullptr;
	m_itemDevBar   = nullptr;
	m_itemDevScale = nullptr;
	m_itemHdgBug   = nullptr;
	m_itemHdgScale = nullptr;

	m_itemCrsText = nullptr;
	m_itemHdgText = nullptr;
	m_itemDmeText = nullptr;

	m_heading    = 0.0f;
	m_headingBug = 0.0f;
	m_course     = 0.0f;
	m_bearing    = 0.0f;
	m_deviation  = 0.0f;
	m_distance   = 0.0f;

	m_bearingVisible   = true;
	m_deviationVisible = true;
	m_distanceVisible  = true;

	m_devBarDeltaX_new = 0.0f;
	m_devBarDeltaX_old = 0.0f;
	m_devBarDeltaY_new = 0.0f;
	m_devBarDeltaY_old = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////

void qfi_NAV::updateView()
{
	OC_METHODGATE();
	m_scaleX = static_cast<float>(width())  /  static_cast<float>(m_originalWidth);
	m_scaleY = static_cast<float>(height()) / static_cast<float>(m_originalHeight);

	m_itemCrsArrow->setRotation( static_cast<qreal>(-m_heading + m_course) );
	m_itemHdgBug->setRotation( static_cast<qreal>(-m_heading + m_headingBug) );
	m_itemHdgScale->setRotation( static_cast<qreal>(-m_heading) );

	if ( m_bearingVisible ) {
		m_itemBrgArrow->setVisible( true );
		m_itemBrgArrow->setRotation( static_cast<qreal>(-m_heading + m_bearing) );
	} else {
		m_itemBrgArrow->setVisible( false );
	}

	if ( m_deviationVisible ) {
		m_itemDevBar->setVisible( true );
		m_itemDevScale->setVisible( true );

		float angle_deg = -m_heading + m_course;
#       ifndef M_PI
		float angle_rad = 3.14159265358979323846 * angle_deg / 180.0f;
#       else
		float angle_rad = static_cast<float>(M_PI) * angle_deg / 180.0f;
#       endif


		float sinAngle = std::sin( angle_rad );
		float cosAngle = std::cos( angle_rad );

		m_itemDevBar->setRotation( static_cast<qreal>(angle_deg) );
		m_itemDevScale->setRotation( static_cast<qreal>(angle_deg) );

		float delta  = m_originalPixPerDev * m_deviation;

		m_devBarDeltaX_new = m_scaleX * delta * cosAngle;
		m_devBarDeltaY_new = m_scaleY * delta * sinAngle;

		m_itemDevBar->moveBy( static_cast<qreal>(m_devBarDeltaX_new - m_devBarDeltaX_old), static_cast<qreal>(m_devBarDeltaY_new - m_devBarDeltaY_old) );
	} else {
		m_itemDevBar->setVisible( false );
		m_itemDevScale->setVisible( false );

		m_devBarDeltaX_new = m_devBarDeltaX_old;
		m_devBarDeltaY_new = m_devBarDeltaY_old;
	}

	m_itemCrsText->setPlainText( QString("CRS %1").arg( static_cast<qreal>(m_course), 3, 'f', 0, QChar('0') ) );
	m_itemHdgText->setPlainText( QString("HDG %1").arg( static_cast<qreal>(m_headingBug), 3, 'f', 0, QChar('0') ) );

	if ( m_distanceVisible ) {
		m_itemDmeText->setVisible( true );
		m_itemDmeText->setPlainText( QString("%1 NM").arg( static_cast<qreal>(m_distance), 5, 'f', 1, QChar(' ') ) );
	} else {
		m_itemDmeText->setVisible( false );
	}

	m_scene->update();

	centerOn( width() / 2.0, height() / 2.0 );
}

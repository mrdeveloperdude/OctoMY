/***************************************************************************//**
 * @file qfi_ASI.cpp
 * @author  Marek M. Cel <marekcel@marekcel.pl>
 *
 * @section LICENSE
 *
 * Copyright (C) 2013 Marek M. Cel
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
 * Copyright (C) 2013 Marek M. Cel
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
#ifndef QFI_ASI_CPP
#define QFI_ASI_CPP
#endif

#include "qfi_ASI.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


#include <QtMath>

////////////////////////////////////////////////////////////////////////////////

qfi_ASI::qfi_ASI( QWidget *parent )
	: QGraphicsView ( parent )

	, m_scene ( nullptr )

	, m_itemFace ( nullptr )
	, m_itemHand ( nullptr )
	, m_itemCase ( nullptr )

	, m_airspeed ( 0.0f )

	, m_scaleX ( 1.0f )
	, m_scaleY ( 1.0f )

	, m_originalHeight ( 240 )
	, m_originalWidth  ( 240 )

	, m_originalAsiCtr ( 120.0, 120.0 )

	, m_faceZ ( -20 )
	, m_handZ ( -10 )
	, m_caseZ (  10 )
	, m_theme("default")
{
	OC_METHODGATE();
	reset();

	m_scene = OC_NEW QGraphicsScene( this );
	setScene( m_scene );

	m_scene->clear();

	init();
}

////////////////////////////////////////////////////////////////////////////////

qfi_ASI::~qfi_ASI()
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

void qfi_ASI::reinit()
{
	OC_METHODGATE();
	if ( m_scene ) {
		m_scene->clear();

		init();
	}
}

////////////////////////////////////////////////////////////////////////////////

void qfi_ASI::update()
{
	OC_METHODGATE();
	updateView();
}



////////////////////////////////////////////////////////////////////////////////

void qfi_ASI::setTheme(QString theme)
{
	OC_METHODGATE();
	m_theme=theme;
}

////////////////////////////////////////////////////////////////////////////////

QString qfi_ASI::theme()
{
	OC_METHODGATE();
	return m_theme;
}


////////////////////////////////////////////////////////////////////////////////

void qfi_ASI::setAirspeed( float airspeed )
{
	OC_METHODGATE();
	m_airspeed = qBound(0.0f, airspeed, 235.0f);
}

////////////////////////////////////////////////////////////////////////////////

void qfi_ASI::resizeEvent( QResizeEvent *event )
{
	OC_METHODGATE();
	////////////////////////////////////
	QGraphicsView::resizeEvent( event );
	////////////////////////////////////

	reinit();
}

////////////////////////////////////////////////////////////////////////////////

void qfi_ASI::init()
{
	OC_METHODGATE();
	m_scaleX = static_cast<float>( width())  / static_cast<float>(m_originalWidth);
	m_scaleY = static_cast<float>( height()) / static_cast<float>(m_originalHeight);

	reset();

	m_itemFace = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/asi/asi_face.svg" );
	m_itemFace->setCacheMode( QGraphicsItem::NoCache );
	m_itemFace->setZValue( m_faceZ );
	m_itemFace->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ), true );
	m_scene->addItem( m_itemFace );

	m_itemHand = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/asi/asi_hand.svg" );
	m_itemHand->setCacheMode( QGraphicsItem::NoCache );
	m_itemHand->setZValue( m_handZ );
	m_itemHand->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ), true );
	m_itemHand->setTransformOriginPoint( m_originalAsiCtr );
	m_scene->addItem( m_itemHand );

	m_itemCase = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/asi/asi_case.svg" );
	m_itemCase->setCacheMode( QGraphicsItem::NoCache );
	m_itemCase->setZValue( m_caseZ );
	m_itemCase->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ), true );
	m_scene->addItem( m_itemCase );

	centerOn( static_cast<qreal>(width() / 2.0f), static_cast<qreal>(height() / 2.0f) );

	updateView();
}

////////////////////////////////////////////////////////////////////////////////

void qfi_ASI::reset()
{
	OC_METHODGATE();
	m_itemFace = nullptr;
	m_itemHand = nullptr;
	m_itemCase = nullptr;

	m_airspeed = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////

void qfi_ASI::updateView()
{
	OC_METHODGATE();
	float angle = 0.0f;

	if ( m_airspeed < 40.0f ) {
		angle = 0.9f * m_airspeed;
	} else if ( m_airspeed < 70.0f ) {
		angle = 36.0f + 1.8f * ( m_airspeed - 40.0f );
	} else if ( m_airspeed < 130.0f ) {
		angle = 90.0f + 2.0f * ( m_airspeed - 70.0f );
	} else if ( m_airspeed < 160.0f ) {
		angle = 210.0f + 1.8f * ( m_airspeed - 130.0f );
	} else {
		angle = 264.0f + 1.2f * ( m_airspeed - 160.0f );
	}

	m_itemHand->setRotation( static_cast<qreal>(angle) );

	m_scene->update();
}

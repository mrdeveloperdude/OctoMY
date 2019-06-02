/***************************************************************************//**
 * @file qfi_ALT.cpp
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
#ifndef QFI_ALT_CPP
#define QFI_ALT_CPP
#endif

#include "qfi_ALT.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QtMath>


////////////////////////////////////////////////////////////////////////////////

qfi_ALT::qfi_ALT( QWidget *parent )
	: QGraphicsView ( parent )

	, m_scene ( nullptr )

	, m_itemFace_1 ( nullptr )
	, m_itemFace_2 ( nullptr )
	, m_itemFace_3 ( nullptr )
	, m_itemHand_1 ( nullptr )
	, m_itemHand_2 ( nullptr )
	, m_itemCase   ( nullptr )

	, m_altitude (  0.0f )
	, m_pressure ( 28.0f )

	, m_scaleX ( 1.0f )
	, m_scaleY ( 1.0f )

	, m_originalHeight ( 240 )
	, m_originalWidth  ( 240 )

	, m_originalAltCtr ( 120.0, 120.0)

	, m_face1Z ( -50 )
	, m_face2Z ( -40 )
	, m_face3Z ( -30 )
	, m_hand1Z ( -20 )
	, m_hand2Z ( -10 )
	, m_caseZ  (  10 )
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

qfi_ALT::~qfi_ALT()
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

void qfi_ALT::reinit()
{
	OC_METHODGATE();
	if ( m_scene ) {
		m_scene->clear();

		init();
	}
}

////////////////////////////////////////////////////////////////////////////////

void qfi_ALT::update()
{
	OC_METHODGATE();
	updateView();
}



////////////////////////////////////////////////////////////////////////////////

void qfi_ALT::setTheme(QString theme)
{
	OC_METHODGATE();
	m_theme=theme;
}

////////////////////////////////////////////////////////////////////////////////

QString qfi_ALT::theme()
{
	OC_METHODGATE();
	return m_theme;
}


////////////////////////////////////////////////////////////////////////////////

void qfi_ALT::setAltitude( float altitude )
{
	OC_METHODGATE();
	m_altitude = altitude;
}

////////////////////////////////////////////////////////////////////////////////

void qfi_ALT::setPressure( float pressure )
{
	OC_METHODGATE();
	m_pressure = pressure;

	if ( m_pressure < 28.0f ) {
		m_pressure = 28.0f;
	}
	if ( m_pressure > 31.5f ) {
		m_pressure = 31.5f;
	}
}

////////////////////////////////////////////////////////////////////////////////

void qfi_ALT::resizeEvent( QResizeEvent *event )
{
	OC_METHODGATE();
	////////////////////////////////////
	QGraphicsView::resizeEvent( event );
	////////////////////////////////////

	reinit();
}

////////////////////////////////////////////////////////////////////////////////

void qfi_ALT::init()
{
	OC_METHODGATE();
	m_scaleX = static_cast<float>(width())  / static_cast<float>(m_originalWidth);
	m_scaleY = static_cast<float>(height()) / static_cast<float>(m_originalHeight);

	reset();

	m_itemFace_1 = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/alt/alt_face_1.svg" );
	m_itemFace_1->setCacheMode( QGraphicsItem::NoCache );
	m_itemFace_1->setZValue( m_face1Z );
	m_itemFace_1->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ), true );
	m_itemFace_1->setTransformOriginPoint( m_originalAltCtr );
	m_scene->addItem( m_itemFace_1 );

	m_itemFace_2 = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/alt/alt_face_2.svg" );
	m_itemFace_2->setCacheMode( QGraphicsItem::NoCache );
	m_itemFace_2->setZValue( m_face2Z );
	m_itemFace_2->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY)  ), true );
	m_scene->addItem( m_itemFace_2 );

	m_itemFace_3 = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/alt/alt_face_3.svg" );
	m_itemFace_3->setCacheMode( QGraphicsItem::NoCache );
	m_itemFace_3->setZValue( m_face3Z );
	m_itemFace_3->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ), true );
	m_itemFace_3->setTransformOriginPoint( m_originalAltCtr );
	m_scene->addItem( m_itemFace_3 );

	m_itemHand_1 = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/alt/alt_hand_1.svg" );
	m_itemHand_1->setCacheMode( QGraphicsItem::NoCache );
	m_itemHand_1->setZValue( m_hand1Z );
	m_itemHand_1->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY)  ), true );
	m_itemHand_1->setTransformOriginPoint( m_originalAltCtr );
	m_scene->addItem( m_itemHand_1 );

	m_itemHand_2 = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/alt/alt_hand_2.svg" );
	m_itemHand_2->setCacheMode( QGraphicsItem::NoCache );
	m_itemHand_2->setZValue( m_hand2Z );
	m_itemHand_2->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY)  ), true );
	m_itemHand_2->setTransformOriginPoint( m_originalAltCtr );
	m_scene->addItem( m_itemHand_2 );

	m_itemCase = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/alt/alt_case.svg" );
	m_itemCase->setCacheMode( QGraphicsItem::NoCache );
	m_itemCase->setZValue( m_caseZ );
	m_itemCase->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY)  ), true );
	m_scene->addItem( m_itemCase );

	centerOn( width() / 2.0, height() / 2.0 );

	updateView();
}

////////////////////////////////////////////////////////////////////////////////

void qfi_ALT::reset()
{
	OC_METHODGATE();
	// TODO: Memory management?
	m_itemFace_1 = nullptr;
	m_itemFace_2 = nullptr;
	m_itemFace_3 = nullptr;
	m_itemHand_1 = nullptr;
	m_itemHand_2 = nullptr;
	m_itemCase   = nullptr;

	m_altitude =  0.0f;
	m_pressure = 28.0f;
}

////////////////////////////////////////////////////////////////////////////////

void qfi_ALT::updateView()
{
	OC_METHODGATE();
	int altitude = static_cast<int>(qCeil( static_cast<qreal>(m_altitude + 0.5f) ));

	float angleH1 = m_altitude * 0.036f;
	float angleH2 = ( altitude % 1000 ) * 0.36f;
	float angleF1 = ( m_pressure - 28.0f ) * 100.0f;
	float angleF3 = m_altitude * 0.0036f;

	m_itemHand_1->setRotation( static_cast<qreal>( angleH1 ));
	m_itemHand_2->setRotation( static_cast<qreal>( angleH2 ));
	m_itemFace_1->setRotation( static_cast<qreal>(-angleF1 ));
	m_itemFace_3->setRotation( static_cast<qreal>( angleF3 ));

	m_scene->update();
}

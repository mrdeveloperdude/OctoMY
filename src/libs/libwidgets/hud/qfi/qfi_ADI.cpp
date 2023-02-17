/***************************************************************************//**
 * @file qfi_ADI.cpp
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
#ifndef QFI_ADI_CPP
#define QFI_ADI_CPP
#endif

////////////////////////////////////////////////////////////////////////////////

#include "qfi_ADI.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QtMath>

////////////////////////////////////////////////////////////////////////////////

qfi_ADI::qfi_ADI( QWidget *parent )
	: QGraphicsView ( parent )

	, m_scene ( nullptr )

	, m_itemBack ( nullptr )
	, m_itemFace ( nullptr )
	, m_itemRing ( nullptr )
	, m_itemCase ( nullptr )

	, m_roll  ( 0.0f )
	, m_pitch ( 0.0f )

	, m_faceDeltaX_new ( 0.0f )
	, m_faceDeltaX_old ( 0.0f )
	, m_faceDeltaY_new ( 0.0f )
	, m_faceDeltaY_old ( 0.0f )

	, m_scaleX ( 1.0f )
	, m_scaleY ( 1.0f )

	, m_originalHeight ( 240 )
	, m_originalWidth  ( 240 )

	, m_originalPixPerDeg ( 1.7f )

	, m_originalAdiCtr ( 120.0, 120.0 )

	, m_backZ ( -30 )
	, m_faceZ ( -20 )
	, m_ringZ ( -10 )
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

qfi_ADI::~qfi_ADI()
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

void qfi_ADI::reinit()
{
	OC_METHODGATE();
	if ( m_scene ) {
		m_scene->clear();

		init();
	}
}

////////////////////////////////////////////////////////////////////////////////

void qfi_ADI::update()
{
	OC_METHODGATE();
	updateView();

	m_faceDeltaX_old  = m_faceDeltaX_new;
	m_faceDeltaY_old  = m_faceDeltaY_new;
}



////////////////////////////////////////////////////////////////////////////////

void qfi_ADI::setTheme(QString theme)
{
	OC_METHODGATE();
	m_theme=theme;
}

////////////////////////////////////////////////////////////////////////////////

QString qfi_ADI::theme()
{
	OC_METHODGATE();
	return m_theme;
}


////////////////////////////////////////////////////////////////////////////////

void qfi_ADI::setRoll( float roll )
{
	OC_METHODGATE();
	m_roll = roll;

	if ( m_roll < -180.0f ) {
		m_roll = -180.0f;
	}
	if ( m_roll >  180.0f ) {
		m_roll =  180.0f;
	}
}

////////////////////////////////////////////////////////////////////////////////

void qfi_ADI::setPitch( float pitch )
{
	OC_METHODGATE();
	m_pitch = pitch;

	if ( m_pitch < -25.0f ) {
		m_pitch = -25.0f;
	}
	if ( m_pitch >  25.0f ) {
		m_pitch =  25.0f;
	}
}

////////////////////////////////////////////////////////////////////////////////

void qfi_ADI::resizeEvent( QResizeEvent *event )
{
	OC_METHODGATE();
	////////////////////////////////////
	QGraphicsView::resizeEvent( event );
	////////////////////////////////////

	reinit();
}

////////////////////////////////////////////////////////////////////////////////

void qfi_ADI::init()
{
	OC_METHODGATE();
	m_scaleX = static_cast<float>(width())  / static_cast<float>(m_originalWidth);
	m_scaleY = static_cast<float>(height()) / static_cast<float>(m_originalHeight);

	reset();

	m_itemBack = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/adi/adi_back.svg" );
	m_itemBack->setCacheMode( QGraphicsItem::NoCache );
	m_itemBack->setZValue( m_backZ );
	m_itemBack->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ), true );
	m_itemBack->setTransformOriginPoint( m_originalAdiCtr );
	m_scene->addItem( m_itemBack );

	m_itemFace = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/adi/adi_face.svg" );
	m_itemFace->setCacheMode( QGraphicsItem::NoCache );
	m_itemFace->setZValue( m_faceZ );
	m_itemFace->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ), true );
	m_itemFace->setTransformOriginPoint( m_originalAdiCtr );
	m_scene->addItem( m_itemFace );

	m_itemRing = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/adi/adi_ring.svg" );
	m_itemRing->setCacheMode( QGraphicsItem::NoCache );
	m_itemRing->setZValue( m_ringZ );
	m_itemRing->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ), true );
	m_itemRing->setTransformOriginPoint( m_originalAdiCtr );
	m_scene->addItem( m_itemRing );

	m_itemCase = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/adi/adi_case.svg" );
	m_itemCase->setCacheMode( QGraphicsItem::NoCache );
	m_itemCase->setZValue( m_caseZ );
	m_itemCase->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ), true );
	m_scene->addItem( m_itemCase );

	centerOn( width() / 2.0, height() / 2.0 );

	updateView();
}

////////////////////////////////////////////////////////////////////////////////

void qfi_ADI::reset()
{
	OC_METHODGATE();
	m_itemBack = nullptr;
	m_itemFace = nullptr;
	m_itemRing = nullptr;
	m_itemCase = nullptr;

	m_roll  = 0.0f;
	m_pitch = 0.0f;

	m_faceDeltaX_new = 0.0f;
	m_faceDeltaX_old = 0.0f;
	m_faceDeltaY_new = 0.0f;
	m_faceDeltaY_old = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////

void qfi_ADI::updateView()
{
	OC_METHODGATE();
	m_scaleX = static_cast<float>(width())  / static_cast<float>(m_originalWidth);
	m_scaleY = static_cast<float>(height()) / static_cast<float>(m_originalHeight);

	m_itemBack->setRotation( static_cast<qreal>(- m_roll ));
	m_itemFace->setRotation( static_cast<qreal>(- m_roll ));
	m_itemRing->setRotation( static_cast<qreal>(- m_roll ));

	float roll_rad = static_cast<float>(M_PI) * m_roll / 180.0f;

	float delta  = m_originalPixPerDeg * m_pitch;

	m_faceDeltaX_new = m_scaleX * delta * std::sin( roll_rad );
	m_faceDeltaY_new = m_scaleY * delta * std::cos( roll_rad );

	m_itemFace->moveBy( static_cast<qreal>(m_faceDeltaX_new - m_faceDeltaX_old), static_cast<qreal>(m_faceDeltaY_new - m_faceDeltaY_old ));

	m_scene->update();
}


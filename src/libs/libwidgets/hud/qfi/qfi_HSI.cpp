/***************************************************************************//**
 * @file qfi_HSI.cpp
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
#ifndef QFI_HSI_CPP
#define QFI_HSI_CPP
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#   include <float.h>
#endif

#include <math.h>

#include "qfi_HSI.h"


#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


////////////////////////////////////////////////////////////////////////////////

qfi_HSI::qfi_HSI( QWidget *parent )
	: QGraphicsView ( parent )

	, m_scene ( nullptr )

	, m_itemFace ( nullptr )
	, m_itemCase ( nullptr )

	, m_heading ( 0.0f )

	, m_scaleX ( 1.0f )
	, m_scaleY ( 1.0f )

	, m_originalHeight ( 240 )
	, m_originalWidth  ( 240 )

	, m_originalHsiCtr ( 120.0, 120.0 )

	, m_faceZ ( -20 )
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

qfi_HSI::~qfi_HSI()
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

void qfi_HSI::reinit()
{
	OC_METHODGATE();
	if ( m_scene ) {
		m_scene->clear();

		init();
	}
}

////////////////////////////////////////////////////////////////////////////////

void qfi_HSI::update()
{
	OC_METHODGATE();
	updateView();
}



////////////////////////////////////////////////////////////////////////////////

void qfi_HSI::setTheme(QString theme)
{
	OC_METHODGATE();
	m_theme=theme;
}

////////////////////////////////////////////////////////////////////////////////

QString qfi_HSI::theme()
{
	OC_METHODGATE();
	return m_theme;
}



////////////////////////////////////////////////////////////////////////////////

void qfi_HSI::setHeading( float heading )
{
	OC_METHODGATE();
	m_heading = heading;
}

////////////////////////////////////////////////////////////////////////////////

void qfi_HSI::resizeEvent( QResizeEvent *event )
{
	OC_METHODGATE();
	////////////////////////////////////
	QGraphicsView::resizeEvent( event );
	////////////////////////////////////

	reinit();
}

////////////////////////////////////////////////////////////////////////////////

void qfi_HSI::init()
{
	OC_METHODGATE();
	m_scaleX = static_cast<float>(width())  / static_cast<float>(m_originalWidth);
	m_scaleY = static_cast<float>(height()) / static_cast<float>(m_originalHeight);

	reset();

	m_itemFace = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/hsi/hsi_face.svg" );
	m_itemFace->setCacheMode( QGraphicsItem::NoCache );
	m_itemFace->setZValue( m_faceZ );
	m_itemFace->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ), true );
	m_itemFace->setTransformOriginPoint( m_originalHsiCtr );
	m_scene->addItem( m_itemFace );

	m_itemCase = OC_NEW QGraphicsSvgItem( ":/qfi/"+m_theme+"/hsi/hsi_case.svg" );
	m_itemCase->setCacheMode( QGraphicsItem::NoCache );
	m_itemCase->setZValue( m_caseZ );
	m_itemCase->setTransform( QTransform::fromScale( static_cast<qreal>(m_scaleX), static_cast<qreal>(m_scaleY) ), true );
	m_scene->addItem( m_itemCase );

	centerOn( width() / 2.0, height() / 2.0 );

	updateView();
}

////////////////////////////////////////////////////////////////////////////////

void qfi_HSI::reset()
{
	OC_METHODGATE();
	m_itemFace = nullptr;
	m_itemCase = nullptr;

	m_heading = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////

void qfi_HSI::updateView()
{
	OC_METHODGATE();
	m_itemFace->setRotation( static_cast<qreal>(- m_heading) );
	m_scene->update();
}


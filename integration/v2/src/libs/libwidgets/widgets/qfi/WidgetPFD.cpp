/***************************************************************************//**
 * @file example/WidgetPFD.cpp
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
#ifndef WIDGETPFD_CPP
#define WIDGETPFD_CPP
#endif

#include "WidgetPFD.hpp"
#include "ui_WidgetPFD.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

////////////////////////////////////////////////////////////////////////////////

WidgetPFD::WidgetPFD( QWidget *parent )
	: QWidget( parent )
	, m_ui( OC_NEW Ui::WidgetPFD )
	, m_pfd ( nullptr )
	, m_layoutSq ( nullptr )
{
	OC_METHODGATE();
	Q_INIT_RESOURCE(qfi);
	m_ui->setupUi( this );

	setupUi();

	m_pfd = m_ui->graphicsPFD;
}

////////////////////////////////////////////////////////////////////////////////

WidgetPFD::~WidgetPFD()
{
	OC_METHODGATE();
	if ( nullptr != m_layoutSq ) {
		delete m_layoutSq;
		m_layoutSq = nullptr;
	}

	if ( nullptr != m_ui ) {
		delete m_ui;
		m_ui = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////

void WidgetPFD::setupUi()
{
	OC_METHODGATE();
	m_layoutSq = OC_NEW LayoutSquare( this );

	m_layoutSq->setContentsMargins( 0, 0, 0, 0 );
	m_layoutSq->addWidget( m_ui->framePFD );

	setLayout( m_layoutSq );
}




void WidgetPFD::update()
{
	OC_METHODGATE();
	m_pfd->update();
}

void WidgetPFD::setRoll( float roll )
{
	OC_METHODGATE();
	m_pfd->setRoll( static_cast<qreal>(roll) );
}

void WidgetPFD::setPitch( float pitch )
{
	OC_METHODGATE();
	m_pfd->setPitch( static_cast<qreal>(pitch) );
}

void WidgetPFD::setFlightPathMarker( float aoa, float sideslip )
{
	OC_METHODGATE();
	m_pfd->setFlightPathMarker( static_cast<qreal>(aoa), static_cast<qreal>(sideslip) );
}

void WidgetPFD::setSlipSkid( float slipSkid )
{
	OC_METHODGATE();
	m_pfd->setSlipSkid( static_cast<qreal>(slipSkid) );
}

void WidgetPFD::setDevH( float devH )
{
	OC_METHODGATE();
	m_pfd->setBarH( static_cast<qreal>(devH) );
	m_pfd->setDotH( static_cast<qreal>(devH) );
}

void WidgetPFD::setDevV( float devV )
{
	OC_METHODGATE();
	m_pfd->setBarV( static_cast<qreal>(devV) );
	m_pfd->setDotV( static_cast<qreal>(devV) );
}

void WidgetPFD::setAltitude( float altitude )
{
	OC_METHODGATE();
	m_pfd->setAltitude( static_cast<qreal>(altitude) );
}

void WidgetPFD::setPressure( float pressure )
{
	OC_METHODGATE();
	m_pfd->setPressure( static_cast<qreal>(pressure), qfi_PFD::IN );
}

void WidgetPFD::setAirspeed( float airspeed )
{
	OC_METHODGATE();
	m_pfd->setAirspeed( static_cast<qreal>(airspeed) );
}

void WidgetPFD::setMachNo( float machNo )
{
	OC_METHODGATE();
	m_pfd->setMachNo( static_cast<qreal>(machNo) );
}

void WidgetPFD::setHeading( float heading )
{
	OC_METHODGATE();
	m_pfd->setHeading( static_cast<qreal>(heading) );
}

void WidgetPFD::setTurnRate( float turnRate )
{
	OC_METHODGATE();
	m_pfd->setTurnRate( static_cast<qreal>(turnRate) );
}

void WidgetPFD::setClimbRate( float climbRate )
{
	OC_METHODGATE();
	m_pfd->setClimbRate( static_cast<qreal>(climbRate) );
}
void WidgetPFD::setTheme(QString theme)
{
	OC_METHODGATE();
	m_pfd->setTheme(theme);
}

QString WidgetPFD::theme()
{
	OC_METHODGATE();
	return m_pfd->theme();
}


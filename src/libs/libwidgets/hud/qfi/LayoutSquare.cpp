/***************************************************************************//**
 * @file example/LayoutSquare.cpp
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
#ifndef LAYOUTSQUARE_CPP
#define LAYOUTSQUARE_CPP
#endif

////////////////////////////////////////////////////////////////////////////////

#include "LayoutSquare.h"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"


////////////////////////////////////////////////////////////////////////////////

LayoutSquare::LayoutSquare( QWidget *parent, int spacing )
	: QLayout ( parent )
{
	OC_METHODGATE();
	init( spacing );
}

////////////////////////////////////////////////////////////////////////////////

LayoutSquare::LayoutSquare( int spacing ) :
	QLayout ( nullptr )
{
	OC_METHODGATE();
	init( spacing );
}

////////////////////////////////////////////////////////////////////////////////

LayoutSquare::~LayoutSquare()
{
	OC_METHODGATE();
	delete m_item;
	m_item = nullptr;
	delete m_rectLast;
	m_rectLast = nullptr;
	delete m_geometry;
	m_geometry = nullptr;
}

////////////////////////////////////////////////////////////////////////////////

void LayoutSquare::addItem( QLayoutItem *item )
{
	OC_METHODGATE();
	if ( !hasItem() ) {
		replaceItem( item );
	}
}

////////////////////////////////////////////////////////////////////////////////

void LayoutSquare::addWidget( QWidget *widget )
{
	OC_METHODGATE();
	if ( !hasItem() ) {
		replaceItem( OC_NEW QWidgetItem( widget ) );
	}
}

////////////////////////////////////////////////////////////////////////////////

int LayoutSquare::count() const
{
	OC_METHODGATE();
	if ( hasItem() ) {
		return 1;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////

Qt::Orientations LayoutSquare::expandingDirections() const
{
	OC_METHODGATE();
	return ( Qt::Horizontal | Qt::Vertical );
}

////////////////////////////////////////////////////////////////////////////////

QRect LayoutSquare::geometry() const
{
	OC_METHODGATE();
	return static_cast<QRect>(*m_geometry);
}

////////////////////////////////////////////////////////////////////////////////

bool LayoutSquare::hasHeightForWidth() const
{
	OC_METHODGATE();
	return false;
}

////////////////////////////////////////////////////////////////////////////////

bool LayoutSquare::hasItem() const
{
	OC_METHODGATE();
	return ( nullptr != m_item );
}

////////////////////////////////////////////////////////////////////////////////

QLayoutItem* LayoutSquare::itemAt( int index ) const
{
	OC_METHODGATE();
	if ( index == 0 ) {
		if ( hasItem() ) {
			return m_item;
		}
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////

QSize LayoutSquare::minimumSize() const
{
	OC_METHODGATE();
	return m_item->minimumSize();
}

////////////////////////////////////////////////////////////////////////////////

QLayoutItem* LayoutSquare::replaceItem( QLayoutItem *item )
{
	OC_METHODGATE();
	QLayoutItem *tempItem = nullptr;

	if ( hasItem() ) {
		tempItem = m_item;
	}

	m_item = item;

	setGeometry( *m_geometry );

	return tempItem;
}

////////////////////////////////////////////////////////////////////////////////

void LayoutSquare::setGeometry( const QRect &rect )
{
	OC_METHODGATE();

	if ( !hasItem() || areRectsEqual( *m_rectLast, rect ) ) {
		return;
	}

	setRectLast( rect );

	QSize  properSize  = calculateProperSize( rect.size() );
	QPoint centerPoint = calculateCenterPnt( rect.size(), properSize );

	m_item->setGeometry( QRect( centerPoint, properSize ) );

	QRect *tempRect = m_geometry;

	m_geometry = OC_NEW QRect( centerPoint, properSize );

	delete tempRect;

	///////////////////////////////////
	QLayout::setGeometry( *m_geometry );
	///////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////

QSize LayoutSquare::sizeHint() const
{
	OC_METHODGATE();
	return m_item->minimumSize();
}

////////////////////////////////////////////////////////////////////////////////

QLayoutItem* LayoutSquare::take()
{
	OC_METHODGATE();
	QLayoutItem *tempItem = nullptr;

	if ( hasItem() ) {
		tempItem = m_item;
		m_item = nullptr;
	}

	return tempItem;
}

////////////////////////////////////////////////////////////////////////////////

QLayoutItem* LayoutSquare::takeAt( int index )
{
	OC_METHODGATE();
	if( 0 == index ) {
		return take();
	}

	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////

bool LayoutSquare::areRectsEqual( const QRect &rect_1, const QRect &rect_2 ) const
{
	OC_METHODGATE();
	bool result = false;

	if (    ( rect_1.x()      == rect_2.x()      )
			&& ( rect_1.y()      == rect_2.y()      )
			&& ( rect_1.height() == rect_2.height() )
			&& ( rect_1.width()  == rect_2.width()  ) ) {
		result = true;
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////////

QPoint LayoutSquare::calculateCenterPnt( QSize fromSize, QSize itemSize ) const
{
	OC_METHODGATE();
	QPoint centerPoint;

	if ( ( fromSize.width() - fromSize.width() / 2.0 - itemSize.width() / 2.0 ) > 0.0 ) {
		centerPoint.setX( static_cast<int>(fromSize.width() - fromSize.width() / 2.0 - itemSize.width() / 2.0 ));
	}

	if ( ( fromSize.height() - fromSize.height() / 2.0 - itemSize.height() / 2.0 ) > 0.0 ) {
		centerPoint.setY( static_cast<int>(fromSize.height() - fromSize.height() / 2.0 - itemSize.height() / 2.0 ));
	}

	return centerPoint;
}

////////////////////////////////////////////////////////////////////////////////

QSize LayoutSquare::calculateProperSize( QSize fromSize ) const
{
	OC_METHODGATE();
	QSize resultSize;
	int l=0, t=0, r=0, b=0;
	getContentsMargins(&l, &t, &r, &b);
	if ( fromSize.height() < fromSize.width() ) {
		resultSize.setHeight ( fromSize.height() - (t+b) );
		resultSize.setWidth  ( fromSize.height() - (l+r) );
	} else {
		resultSize.setHeight ( fromSize.width() - (t+b) );
		resultSize.setWidth  ( fromSize.width() - (l+r) );
	}

	return resultSize;
}

////////////////////////////////////////////////////////////////////////////////

void LayoutSquare::init( int spacing )
{
	OC_METHODGATE();
	m_item = nullptr;

	m_rectLast = OC_NEW QRect( 0, 0, 0, 0 );
	m_geometry = OC_NEW QRect( 0, 0, 0, 0 );

	setSpacing( spacing );
}

////////////////////////////////////////////////////////////////////////////////

void LayoutSquare::setRectLast( const QRect &rect )
{
	OC_METHODGATE();
	QRect *tempRect = m_rectLast;

	m_rectLast = OC_NEW QRect( rect.topLeft(), rect.size() );

	delete tempRect;
}

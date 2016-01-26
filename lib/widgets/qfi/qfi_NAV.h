/***************************************************************************//**
 * @file qfi_NAV.h
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
#ifndef QFI_NAV_H
#define QFI_NAV_H

////////////////////////////////////////////////////////////////////////////////

#include <QGraphicsView>
#include <QGraphicsSvgItem>

////////////////////////////////////////////////////////////////////////////////

/** This class provides Navigation Display widget. */
class qfi_NAV : public QGraphicsView
{
    Q_OBJECT

public:

    /** Constructor. */
    explicit qfi_NAV( QWidget *parent = 0 );

    /** Destructor. */
    virtual ~qfi_NAV();

    /** Reinitiates widget. */
    void reinit();

    /** Refreshes (redraws) widget. */
    void update();

    /** @param heading [deg] */
    void setHeading( float heading );

    /** @param heading bug [deg] */
    void setHeadingBug( float headingBug );

    /** @param course [deg] */
    void setCourse( float course );

    /** @param bearing [deg] */
    void setBearing( float bearing, bool visible = false );

    /** @param deviation [-] */
    void setDeviation( float deviation, bool visible = false );

    /** @param distance [nm] */
    void setDistance( float distance, bool visible = false );

protected:

    /** */
    void resizeEvent( QResizeEvent *event );

private:

    QGraphicsScene *m_scene;            ///< graphics scene

    QGraphicsSvgItem *m_itemBack;       ///< NAV background
    QGraphicsSvgItem *m_itemMask;       ///< NAV mask
    QGraphicsSvgItem *m_itemMark;

    QGraphicsSvgItem *m_itemBrgArrow;
    QGraphicsSvgItem *m_itemCrsArrow;
    QGraphicsSvgItem *m_itemDevBar;
    QGraphicsSvgItem *m_itemDevScale;
    QGraphicsSvgItem *m_itemHdgBug;
    QGraphicsSvgItem *m_itemHdgScale;

    QGraphicsTextItem *m_itemCrsText;
    QGraphicsTextItem *m_itemHdgText;
    QGraphicsTextItem *m_itemDmeText;

    QColor m_crsTextColor;
    QColor m_hdgTextColor;
    QColor m_dmeTextColor;

    QFont  m_crsTextFont;
    QFont  m_hdgTextFont;
    QFont  m_dmeTextFont;

    float m_heading;                    ///< [deg]
    float m_headingBug;                 ///< [deg]
    float m_course;
    float m_bearing;
    float m_deviation;
    float m_distance;

    bool m_bearingVisible;
    bool m_deviationVisible;
    bool m_distanceVisible;

    float m_devBarDeltaX_new;
    float m_devBarDeltaX_old;
    float m_devBarDeltaY_new;
    float m_devBarDeltaY_old;

    float m_scaleX; ///<
    float m_scaleY; ///<

    float m_originalPixPerDev;

    QPointF m_originalNavCtr;

    QPointF m_originalCrsTextCtr;
    QPointF m_originalHdgTextCtr;
    QPointF m_originalDmeTextCtr;

    const int m_originalHeight;         ///< [px]
    const int m_originalWidth;          ///< [px]

    const int m_backZ;
    const int m_maskZ;
    const int m_markZ;

    const int m_brgArrowZ;
    const int m_crsArrowZ;
    const int m_crsTextZ;
    const int m_devBarZ;
    const int m_devScaleZ;
    const int m_hdgBugZ;
    const int m_hdgScaleZ;
    const int m_hdgTextZ;
    const int m_dmeTextZ;

    /** */
    void init();

    /** */
    void reset();

    /** */
    void updateView();
};

////////////////////////////////////////////////////////////////////////////////

#endif // QFI_NAV_H

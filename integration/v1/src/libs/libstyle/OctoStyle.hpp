/*
 * Octo Style, based on Qt5 fusion style
 * Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies)
 * Copyright (C) 2013-2014 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * For a full copy of the license see the doc/LGPL.txt file
 */

#ifndef OCTO_STYLE_HPP_INCLUDED
#define OCTO_STYLE_HPP_INCLUDED

#include <QtCore/Qt>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
# include <QtWidgets/QCommonStyle>
#else
# include <QtGui/QCommonStyle>
#endif

class OctoStylePrivate;

class OctoStyle : public QCommonStyle
{
	Q_OBJECT
private:
	OctoStylePrivate* const d;

private:
	friend class OctoStylePrivate;

public:
	OctoStyle();
	~OctoStyle();

	QPalette standardPalette() const;
	void drawPrimitive(PrimitiveElement elem, const QStyleOption* option, QPainter* painter, const QWidget* widget = NULL) const;
	void drawControl(ControlElement ce, const QStyleOption* option, QPainter* painter, const QWidget* widget) const;
	int pixelMetric(PixelMetric metric, const QStyleOption* option = NULL, const QWidget* widget = NULL) const;
	void drawComplexControl(ComplexControl control, const QStyleOptionComplex* option, QPainter* painter, const QWidget* widget) const;
	QRect subElementRect(SubElement r, const QStyleOption* opt, const QWidget* widget = NULL) const;
	QSize sizeFromContents(ContentsType type, const QStyleOption* option, const QSize& size, const QWidget* widget) const;
	QRect subControlRect(ComplexControl cc, const QStyleOptionComplex* opt, SubControl sc, const QWidget* widget) const;
	int styleHint(StyleHint hint, const QStyleOption* option = NULL, const QWidget* widget = NULL, QStyleHintReturn* returnData = NULL) const;
	void drawItemText(QPainter *painter, const QRect& rect, int flags, const QPalette& pal, bool enabled, const QString& text, QPalette::ColorRole textRole = QPalette::NoRole) const;
	void polish(QApplication* app);
	void polish(QPalette& pal);
	void polish(QWidget* widget);
	void unpolish(QApplication* app);
	void unpolish(QWidget* widget);


};

#endif // OCTO_STYLE_HPP_INCLUDED

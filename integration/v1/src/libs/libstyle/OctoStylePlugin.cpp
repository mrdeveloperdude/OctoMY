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

#include "OctoStyle.hpp"
#include "utility/Standard.hpp"

#include <QStylePlugin>

class OctoStylePlugin : public QStylePlugin
{
	public:
		QStringList keys() const {
			return QStringList(QLatin1String("Octo"));
		}

		QStyle *create(const QString &key) {
			if (key.toLower() == QLatin1String("octo")) {
				return OC_NEW OctoStyle;
			}
			return 0;
		}
};

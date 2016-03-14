TARGET = map
TEMPLATE = lib
CONFIG +=	staticlib
include($$TOP_PWD/common.pri)

QT += network
QT += gui
QT += widgets


HEADERS +=	\
			arrowpoint.h \
			bingapimapadapter.h \
			circlepoint.h \
			curve.h \
			emptymapadapter.h \
			fixedimageoverlay.h \
			geometry.h \
			geometrylayer.h \
			googleapimapadapter.h \
			googlemapadapter.h \
			gps_position.h \
			imagemanager.h \
			imagepoint.h \
			invisiblepoint.h \
			layer.h \
			layermanager.h \
			linestring.h \
			mapadapter.h \
			mapcontrol.h \
			maplayer.h \
			mapnetwork.h \
			openaerialmapadapter.h \
			osmmapadapter.h \
			point.h \
			qmapcontrol_global.h \
			tilemapadapter.h \
			wmsmapadapter.h \


SOURCES += \
			arrowpoint.cpp \
			bingapimapadapter.cpp \
			circlepoint.cpp \
			curve.cpp \
			emptymapadapter.cpp \
			fixedimageoverlay.cpp \
			geometry.cpp \
			geometrylayer.cpp \
			googleapimapadapter.cpp \
			googlemapadapter.cpp \
			gps_position.cpp \
			imagemanager.cpp \
			imagepoint.cpp \
			invisiblepoint.cpp \
			layer.cpp \
			layermanager.cpp \
			linestring.cpp \
			mapadapter.cpp \
			mapcontrol.cpp \
			maplayer.cpp \
			mapnetwork.cpp \
			openaerialmapadapter.cpp \
			osmmapadapter.cpp \
			point.cpp \
			tilemapadapter.cpp \
			wmsmapadapter.cpp \

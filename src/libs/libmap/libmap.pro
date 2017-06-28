TARGET = map
TEMPLATE = lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)
include($$SRCS/libs/libcomms/libcomms.pri)

INCLUDEPATH += $$SRCS/libs/libmap/


QT += network
QT += gui
QT += widgets

MAP_TOP=$$SRCS/libs/libmap/map

HEADERS +=	\
	$$MAP_TOP/GoogleMapAdapter.hpp \
	$$MAP_TOP/GPSPosition.hpp \
	$$MAP_TOP/ImageManager.hpp \
	$$MAP_TOP/ImagePoint.hpp \
	$$MAP_TOP/InvisiblePoint.hpp \
	$$MAP_TOP/Layer.hpp \
	$$MAP_TOP/LayerManager.hpp \
	$$MAP_TOP/WMSMapAdapter.hpp \
	$$MAP_TOP/TileMapAdapter.hpp \
	$$MAP_TOP/qMapControlGlobal.hpp \
	$$MAP_TOP/Point.hpp \
	$$MAP_TOP/OSMMapAdapter.hpp \
	$$MAP_TOP/OpenAerialMapAdapter.hpp \
	$$MAP_TOP/MapNetwork.hpp \
	$$MAP_TOP/MapLayer.hpp \
	$$MAP_TOP/MapControl.hpp \
	$$MAP_TOP/MapAdapter.hpp \
	$$MAP_TOP/LineString.hpp \
	$$MAP_TOP/GoogleAPIMapAdapter.hpp \
	$$MAP_TOP/GeometryLayer.hpp \
	$$MAP_TOP/Geometry.hpp \
	$$MAP_TOP/FixedImageOverlay.hpp \
	$$MAP_TOP/EmptyMapAdapter.hpp \
	$$MAP_TOP/Curve.hpp \
	$$MAP_TOP/Circlepoint.hpp \
	$$MAP_TOP/BingAPIMapAdapter.hpp \
	$$MAP_TOP/ArrowPoint.hpp \


SOURCES += \
	$$MAP_TOP/ArrowPoint.cpp \
	$$MAP_TOP/BingAPIMapAdapter.cpp \
	$$MAP_TOP/CirclePoint.cpp \
	$$MAP_TOP/Curve.cpp \
	$$MAP_TOP/EmptyMapAdapter.cpp \
	FixedImageOverlay.cpp \
	$$MAP_TOP/Geometry.cpp \
	$$MAP_TOP/GeometryLayer.cpp \
	$$MAP_TOP/GoogleAPIMapAdapter.cpp \
	$$MAP_TOP/GoogleMapAdapter.cpp \
	$$MAP_TOP/GPSPosition.cpp \
	$$MAP_TOP/ImageManager.cpp \
	$$MAP_TOP/ImagePoint.cpp \
	$$MAP_TOP/InvisiblePoint.cpp \
	$$MAP_TOP/Layer.cpp \
	$$MAP_TOP/LayerManager.cpp \
	$$MAP_TOP/LineString.cpp \
	$$MAP_TOP/MapAdapter.cpp \
	$$MAP_TOP/MapControl.cpp \
	$$MAP_TOP/MapLayer.cpp \
	$$MAP_TOP/MapNetwork.cpp \
	$$MAP_TOP/OpenAerialMapAdapter.cpp \
	$$MAP_TOP/OSMMapAdapter.cpp \
	$$MAP_TOP/Point.cpp \
	$$MAP_TOP/TileMapAdapter.cpp \
	$$MAP_TOP/WMSMapAdapter.cpp \



contains(DEFINES, USE_STATUS){
message("FROM libmap.pro")
include($$PRIS/status.pri)
}

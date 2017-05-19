TARGET = map
TEMPLATE = lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib.pri)
include($$TOP_PWD/libs/libcomms/libcomms.pri)

INCLUDEPATH += $$TOP_PWD/libs/libmap/


QT += network
QT += gui
QT += widgets


HEADERS +=	\
	GoogleMapAdapter.hpp \
	GPSPosition.hpp \
	ImageManager.hpp \
	ImagePoint.hpp \
	InvisiblePoint.hpp \
	Layer.hpp \
	LayerManager.hpp \
	WMSMapAdapter.hpp \
	TileMapAdapter.hpp \
	qMapControlGlobal.hpp \
	Point.h \
	OSMMapAdapter.hpp \
	OpenAerialMapAdapter.hpp \
	MapNetwork.hpp \
	MapLayer.hpp \
	MapControl.hpp \
	MapAdapter.hpp \
	LineString.hpp \
	GoogleAPIMapAdapter.hpp \
	GeometryLayer.hpp \
	Geometry.hpp \
	FixedImageOverlay.hpp \
	EmptyMapAdapter.hpp \
	Curve.hpp \
	Circlepoint.hpp \
	BingAPIMapAdapter.hpp \
	ArrowPoint.hpp


SOURCES += \
	ArrowPoint.cpp \
	BingAPIMapAdapter.cpp \
	CirclePoint.cpp \
	Curve.cpp \
	EmptyMapAdapter.cpp \
	FixedImageOverlay.cpp \
	Geometry.cpp \
	GeometryLayer.cpp \
	GoogleAPIMapAdapter.cpp \
	GoogleMapAdapter.cpp \
	GPSPosition.cpp \
	ImageManager.cpp \
	ImagePoint.cpp \
	InvisiblePoint.cpp \
	Layer.cpp \
	LayerManager.cpp \
	LineString.cpp \
	MapAdapter.cpp \
	MapControl.cpp \
	MapLayer.cpp \
	MapNetwork.cpp \
	OpenAerialMapAdapter.cpp \
	OSMMapAdapter.cpp \
	Point.cpp \
	TileMapAdapter.cpp \
	WMSMapAdapter.cpp



contains(DEFINES, USE_STATUS){
message("FROM libmap.pro")
include($$TOP_PWD/status.pri)
}

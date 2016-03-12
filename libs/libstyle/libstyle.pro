TARGET =	style
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)

INCLUDEPATH += .

QT += widgets

HEADERS	+= \
	OctoStyle.hpp \
	OctoStylePrivate.hpp \
	OctoStyleAnimations.hpp

SOURCES	+= \
	OctoStyle.cpp \
	OctoStylePlugin.cpp

RESOURCES += \
	OctoStyle.qrc

DISTFILES += \
	OctoStyle.json



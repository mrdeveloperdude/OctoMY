TARGET =	style
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib.pri)
QT += widgets
INCLUDEPATH += ./


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



contains(DEFINES, USE_STATUS){
message("FROM libstyle.pro:")
include($$TOP_PWD/status.pri)
}

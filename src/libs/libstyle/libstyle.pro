TARGET =	style
TEMPLATE =	lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)

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
include($$PRIS/status.pri)
}

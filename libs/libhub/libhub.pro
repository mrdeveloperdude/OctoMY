TARGET =	hub
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib.pri)

includes= node
for(i, includes) {
	INCLUDEE=$$i
	include($$TOP_PWD/libincluder.pri)
}

#include($$TOP_PWD/libs/libnode/libnode.pri)

I=hub
L=hub

SOURCES +=\
	$$L/Hub.cpp \
	$$L/HubWindow.cpp \
	$$L/IContextProvider.cpp \

HEADERS  += \
	$$I/Hub.hpp \
	$$I/HubWindow.hpp \
	$$I/IContextProvider.hpp \


FORMS	+= \
	ui/HubWindow.ui \

contains(DEFINES, USE_STATUS){
message("FROM libhub.pro:")
include($$TOP_PWD/status.pri)
}


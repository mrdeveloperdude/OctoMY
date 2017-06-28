TARGET =	hub
TEMPLATE =	lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)

includes= node
for(i, includes) {
	INCLUDEE=$$i
	include($$PRIS/libincluder.pri)
}

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
include($$PRIS/status.pri)
}


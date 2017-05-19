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


#QT         += core core-private gui gui-private
INCLUDEPATH += $$TOP_PWD/libs/libhub/

SOURCES +=\
	Hub.cpp \
	HubWindow.cpp \
	IContextProvider.cpp \

HEADERS  += \
	Hub.hpp \
	HubWindow.hpp \
	IContextProvider.hpp \


FORMS	+= \
	ui/HubWindow.ui \

contains(DEFINES, USE_STATUS){
message("FROM libhub.pro:")
include($$TOP_PWD/status.pri)
}


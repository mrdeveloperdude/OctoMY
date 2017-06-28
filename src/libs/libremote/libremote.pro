TARGET =	remote
TEMPLATE =	lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)

includes = node

for(i, includes) {
	INCLUDEE=$$i
	include($$PRIS/libincluder.pri)
}


#include($$SRCS/libs/libnode/libnode.pri)
#include($$SRCS/libs/libcore/libcore.pri)
#QT         += core core-private gui gui-private
INCLUDEPATH += $$TOP_PWD/libs/libremote/


L=remote
I=remote
SOURCES +=\
	$$L/Remote.cpp \
	$$L/RemoteWindow.cpp \

HEADERS  += \
	$$I/Remote.hpp \
	$$I/RemoteWindow.hpp \

FORMS += \
	ui/RemoteWindow.ui \


contains(DEFINES, USE_STATUS){
message("FROM libremote.pro:")
include($$PRIS/status.pri)
}

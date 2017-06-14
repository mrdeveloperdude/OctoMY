TARGET =	remote
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib.pri)

includes = node

for(i, includes) {
	INCLUDEE=$$i
	include($$TOP_PWD/libincluder.pri)
}


#include($$TOP_PWD/libs/libnode/libnode.pri)
#include($$TOP_PWD/libs/libcore/libcore.pri)
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
include($$TOP_PWD/status.pri)
}

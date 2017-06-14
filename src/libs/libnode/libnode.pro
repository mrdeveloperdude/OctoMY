TARGET =	node
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib.pri)

includes = core

for(i, includes) {
	INCLUDEE=$$i
	include($$TOP_PWD/libincluder.pri)
}


#QT         += core core-private gui gui-private
INCLUDEPATH += ./


SOURCES +=\
	node/AppContext.cpp \
	node/ClientWidget.cpp \
	node/Node.cpp \
	node/NodeLauncher.cpp \

HEADERS  += \
	node/AppContext.hpp \
	node/ClientWidget.hpp \
	node/Node.hpp \
	node/NodeLauncher.hpp \


contains(DEFINES, USE_STATUS){
message("FROM libnode.pro:")
include($$TOP_PWD/status.pri)
}

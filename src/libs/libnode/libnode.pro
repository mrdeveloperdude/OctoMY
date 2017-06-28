TARGET =	node
TEMPLATE =	lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)

# NEEDED FOR OpenCL
QT         += core-private gui-private

includes = core util rng comms sec zbar puppet audio glt clt agent map qpolarssl qr web zbar

for(i, includes) {
	INCLUDEE=$$i
	include($$PRIS/libincluder.pri)
}

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
include($$PRIS/status.pri)
}

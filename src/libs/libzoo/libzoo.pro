TARGET =	zoo
TEMPLATE =	lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)

INCLUDEPATH += ./

SOURCES +=\

HEADERS  += \

contains(DEFINES, USE_STATUS){
message("FROM libzoo.pro:")
include($$PRIS/status.pri)
}

TARGET =	zoo
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib.pri)
#QT         += core core-private gui gui-private
INCLUDEPATH += ./


SOURCES +=\




HEADERS  += \


contains(DEFINES, USE_STATUS){
message("FROM libzoo.pro:")
include($$TOP_PWD/status.pri)
}

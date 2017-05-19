TARGET =	web
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib.pri)

include(files.pri)


contains(DEFINES, USE_STATUS){
message("FROM libweb.pro:")
include($$TOP_PWD/status.pri)
}

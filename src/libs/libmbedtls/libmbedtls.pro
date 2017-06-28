TARGET =	mbedtls
TEMPLATE =	lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)

include(files.pri)

contains(DEFINES, USE_STATUS){
message("FROM libmbedtls.pro")
include($$PRIS/status.pri)
}


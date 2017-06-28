TARGET =	qpolarssl
TEMPLATE =	lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)


includes = mbedtls

for(i, includes) {
	INCLUDEE=$$i
	include($$PRIS/libincluder.pri)
}

INCLUDEPATH += ./
		

include(files.pri)

contains(DEFINES, USE_STATUS){
message("FROM libqpolarssl.pro")
include($$PRIS/status.pri)
}

DISTFILES += \
	files.pri

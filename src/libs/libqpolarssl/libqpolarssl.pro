TARGET =	qpolarssl
TEMPLATE =	lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)

L=qpolarssl
I=qpolarssl

INCLUDEPATH += $$SRCS/libs/libqpolarssl/


SOURCES += \
	$$L/qpolarsslcipher.cpp \
	$$L/qpolarsslhash.cpp \
	$$L/qpolarsslpki.cpp \
	$$L/qpolarsslrandom.cpp \

HEADERS += \
	$$I/qpolarsslbase.hpp \
	$$I/qpolarsslcipher.hpp \
	$$I/qpolarsslcipher_priv.hpp \
	$$I/qpolarsslhash.hpp \
	$$I/qpolarsslhash_priv.hpp \
	$$I/qpolarsslpki.hpp \
	$$I/qpolarsslpki_priv.hpp \
	$$I/qpolarsslrandom.hpp \
	$$I/qpolarsslrandom_priv.hpp \
	$$I/qpolarssltypes.hpp

include(mbedtls/files.pri)

contains(DEFINES, USE_STATUS){
message("FROM libqpolarssl.pro")
include($$PRIS/status.pri)
}

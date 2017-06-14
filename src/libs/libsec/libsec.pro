TARGET =	sec
TEMPLATE =	lib
CONFIG +=	staticlib

#QT         += core core-private gui gui-private
INCLUDEPATH += $$TOP_PWD/libs/libsec/

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib.pri)
#include($$TOP_PWD/libs/libmbedtls/libmbedtls.pri)
#include($$TOP_PWD/libs/libqpolarssl/libqpolarssl.pri)

L=security
I=security


SOURCES += \
	$$L/Key.cpp \
	$$L/KeyPrivate.cpp \
	$$L/KeyStore.cpp \
	$$L/PortableID.cpp \
	$$L/SecurityConstants.cpp \
	security/SecurityConstantsPrivate.cpp

HEADERS += \
	$$I/Key.hpp \
	$$I/KeyPrivate.hpp \
	$$I/KeyStore.hpp \
	$$I/PortableID.hpp \
	$$I/SecurityConstants.hpp \
	$$I/SecurityConstantsPrivate.hpp

win32:LIBS += -ladvapi32


contains(DEFINES, USE_STATUS){
message("FROM libsec.pro")
include($$TOP_PWD/status.pri)
}


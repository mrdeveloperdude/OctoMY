TARGET =	sec
TEMPLATE =	lib
CONFIG +=	staticlib



include($$PRIS/common.pri)
include($$PRIS/lib.pri)

INCLUDEPATH += $$SRCS/libs/libsec/

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
include($$PRIS/status.pri)
}


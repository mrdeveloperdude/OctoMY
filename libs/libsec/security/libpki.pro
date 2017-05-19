TARGET =	sec
TEMPLATE =	lib
CONFIG +=	staticlib

SECURITY_DIR=security
include($$TOP_PWD/common.pri)
#QT         += core core-private gui gui-private
INCLUDEPATH += ./

include(mbedtls/libmbedtls.pri)
include(qpolarssl/libqpolarssl.pri)

SOURCES += \
	$$SECURITY_DIR/Key.cpp \
	$$SECURITY_DIR/KeyPrivate.cpp \
	$$SECURITY_DIR/KeyStore.cpp \
	$$SECURITY_DIR/PortableID.cpp \
	$$SECURITY_DIR/SecurityConstants.cpp \

HEADERS += \
	$$SECURITY_DIR/Key.hpp \
	$$SECURITY_DIR/KeyPrivate.hpp \
	$$SECURITY_DIR/KeyStore.hpp \
	$$SECURITY_DIR/PortableID.hpp \
	$$SECURITY_DIR/SecurityConstants.hpp \


win32:LIBS += -ladvapi32


contains(DEFINES, USE_STATUS){
message("FROM libpki.pro")
include($$TOP_PWD/status.pri)
}

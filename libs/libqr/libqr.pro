TARGET =	qr
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib.pri)
#QT         += core core-private gui gui-private
INCLUDEPATH += ./

HEADERS	+= \
	BitBuffer.hpp \
	QrCode.hpp \
	QrSegment.hpp \
	QRPainter.hpp

SOURCES	+= \
	BitBuffer.cpp \
	QrCode.cpp \
	QrSegment.cpp \
	QRPainter.cpp

contains(DEFINES, USE_STATUS){
message("FROM libqfi.pro:")
include($$TOP_PWD/status.pri)
}

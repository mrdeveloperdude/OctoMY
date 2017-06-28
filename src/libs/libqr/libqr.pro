TARGET =	qr
TEMPLATE =	lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)

INCLUDEPATH += ./

TOP_QR=qr/

HEADERS	+= \
	$$TOP_QR/BitBuffer.hpp \
	$$TOP_QR/QrCode.hpp \
	$$TOP_QR/QrSegment.hpp \
	$$TOP_QR/QRPainter.hpp \

SOURCES	+= \
	$$TOP_QR/BitBuffer.cpp \
	$$TOP_QR/QrCode.cpp \
	$$TOP_QR/QrSegment.cpp \
	$$TOP_QR/QRPainter.cpp \

contains(DEFINES, USE_STATUS){
message("FROM libqr.pro:")
include($$PRIS/status.pri)
}

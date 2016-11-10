TARGET =	qr
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)

INCLUDEPATH += ./

HEADERS	+= \
	BitBuffer.hpp \
	QrCode.hpp \
	QrSegment.hpp

SOURCES	+= \
	BitBuffer.cpp \
	QrCode.cpp \
	QrSegment.cpp


TARGET =	qr
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)

INCLUDEPATH += .

HEADERS	+= \
	bitstream.h \
	config.h \
	mask.h \
	mmask.h \
	mqrspec.h \
	qrencode.h \
	qrencode_inner.h \
	qrinput.h \
	qrspec.h \
	rscode.h \
	split.h \

SOURCES	+= \
	bitstream.c \
	mask.c \
	mmask.c \
	mqrspec.c \
	qrenc.c \
	qrencode.c \
	qrinput.c \
	qrspec.c \
	rscode.c \
	split.c \


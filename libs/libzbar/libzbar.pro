TARGET =	zbar
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)

INCLUDEPATH += .

HEADERS	+= \
	config.h \
	debug.h \
	decoder/code128.h \
	decoder/code39.h \
	decoder/ean.h \
	decoder.h \
	decoder/i25.h \
	decoder/qr_finder.h \
	error.h \
	event.h \
	image.h \
	img_scanner.h \
	mutex.h \
	processor.h \
	processor/posix.h \
	qrcode/bch15_5.h \
	qrcode/binarize.h \
	qrcode.h \
	qrcode/isaac.h \
	qrcode/qrdec.h \
	qrcode/rs.h \
	qrcode/util.h \
	refcnt.h \
	svg.h \
	symbol.h \
	thread.h \
	timer.h \
	video.h \
	window.h \
	zbar.h \

SOURCES	+= \
	config.c \
	convert.c \
	decoder.c \
	decoder/code128.c \
	decoder/code39.c \
	decoder/ean.c \
	decoder/i25.c \
	decoder/qr_finder.c \
	error.c \
	image.c \
	img_scanner.c \
	processor.c \
	processor/null.c \
	processor/posix.c \
	qrcode/bch15_5.c \
	qrcode/binarize.c \
	qrcode/isaac.c \
	qrcode/qrdec.c \
	qrcode/rs.c \
	qrcode/util.c \
	refcnt.c \
	scanner.c \
	svg.c \
	symbol.c \
	video.c \
	video/null.c \
	window.c \
	window/null.c \



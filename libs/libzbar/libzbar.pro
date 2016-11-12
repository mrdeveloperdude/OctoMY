TARGET =	zbar
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)

# Since this is in part pure C, we set the desired C standard

QMAKE_CFLAGS += -std=c99

INCLUDEPATH += ./

HEADERS	+= \
	c++/Decoder.h \
	c++/Exception.h \
	c++/Image.hpp \
	c++/ImageScanner.h \
	config.h \
	c++/Processor.h \
	c++/QZBarImage.h \
	c++/Scanner.h \
	c++/Symbol.h \
	c++/Video.h \
	c++/Window.h \
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
	QtIconv.h \
	refcnt.h \
	svg.h \
	symbol.h \
	thread.h \
	timer.h \
	video.h \
	window.h \
	zbar.h \
	ZBarScanner.hpp \



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
	img_scanner.c \
	processor.c \
	processor/posix.c \
	processor/processor_null.c \
	qrcode/bch15_5.c \
	qrcode/binarize.c \
	qrcode/isaac.c \
	qrcode/qrdec.c \
	qrcode/qrdectxt.c \
	qrcode/rs.c \
	qrcode/util.c \
	QtIconv.cpp \
	refcnt.c \
	scanner.c \
	svg.c \
	symbol.c \
	video.c \
	video/video_null.c \
	window.c \
	window/window_null.c \
	ZBarScanner.cpp \
	zimage.c \


# There is no support for iconv in Android, so we bring our own as QtIconv.?pp


TARGET =	zbar
TEMPLATE =	lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)

INCLUDEPATH += ./
# CHEEKY HACK YO *<8o)
includes = util

for(i, includes) {
	INCLUDEE=$$i
	include($$PRIS/libincluder.pri)
}


TOP_ZBAR=zbar

HEADERS	+= \
	$$TOP_ZBAR/c++/Decoder.h \
	$$TOP_ZBAR/c++/Exception.h \
	$$TOP_ZBAR/c++/Image.hpp \
	$$TOP_ZBAR/c++/ImageScanner.h \
	$$TOP_ZBAR/config.h \
	$$TOP_ZBAR/c++/Processor.h \
	$$TOP_ZBAR/c++/QZBarImage.h \
	$$TOP_ZBAR/c++/Scanner.h \
	$$TOP_ZBAR/c++/Symbol.h \
	$$TOP_ZBAR/c++/Video.h \
	$$TOP_ZBAR/c++/Window.h \
	$$TOP_ZBAR/debug.h \
	$$TOP_ZBAR/decoder/code128.h \
	$$TOP_ZBAR/decoder/code39.h \
	$$TOP_ZBAR/decoder/ean.h \
	$$TOP_ZBAR/decoder.h \
	$$TOP_ZBAR/decoder/i25.h \
	$$TOP_ZBAR/decoder/qr_finder.h \
	$$TOP_ZBAR/zbar_error.h \
	$$TOP_ZBAR/event.h \
	$$TOP_ZBAR/image.h \
	$$TOP_ZBAR/img_scanner.h \
	$$TOP_ZBAR/mutex.h \
	$$TOP_ZBAR/processor.h \
	$$TOP_ZBAR/processor/posix.h \
	$$TOP_ZBAR/qrcode/bch15_5.h \
	$$TOP_ZBAR/qrcode/binarize.h \
	$$TOP_ZBAR/qrcode.h \
	$$TOP_ZBAR/qrcode/isaac.h \
	$$TOP_ZBAR/qrcode/qrdec.h \
	$$TOP_ZBAR/qrcode/rs.h \
	$$TOP_ZBAR/qrcode/util.h \
	$$TOP_ZBAR/QtIconv.h \
	$$TOP_ZBAR/refcnt.h \
	$$TOP_ZBAR/svg.h \
	$$TOP_ZBAR/symbol.h \
	$$TOP_ZBAR/thread.h \
	$$TOP_ZBAR/timer.h \
	$$TOP_ZBAR/video.h \
	$$TOP_ZBAR/window.h \
	$$TOP_ZBAR/zbar.h \
	$$TOP_ZBAR/ZBarScanner.hpp \



SOURCES	+= \
	$$TOP_ZBAR/config.c \
	$$TOP_ZBAR/convert.c \
	$$TOP_ZBAR/decoder.c \
	$$TOP_ZBAR/decoder/code128.c \
	$$TOP_ZBAR/decoder/code39.c \
	$$TOP_ZBAR/decoder/ean.c \
	$$TOP_ZBAR/decoder/i25.c \
	$$TOP_ZBAR/decoder/qr_finder.c \
	$$TOP_ZBAR/zbar_error.c \
	$$TOP_ZBAR/img_scanner.c \
	$$TOP_ZBAR/processor.c \
	$$TOP_ZBAR/processor/posix.c \
	$$TOP_ZBAR/processor/processor_null.c \
	$$TOP_ZBAR/qrcode/bch15_5.c \
	$$TOP_ZBAR/qrcode/binarize.c \
	$$TOP_ZBAR/qrcode/isaac.c \
	$$TOP_ZBAR/qrcode/qrdec.c \
	$$TOP_ZBAR/qrcode/qrdectxt.c \
	$$TOP_ZBAR/qrcode/rs.c \
	$$TOP_ZBAR/qrcode/util.c \
	$$TOP_ZBAR/QtIconv.cpp \
	$$TOP_ZBAR/refcnt.c \
	$$TOP_ZBAR/scanner.c \
	$$TOP_ZBAR/svg.c \
	$$TOP_ZBAR/symbol.c \
	$$TOP_ZBAR/video.c \
	$$TOP_ZBAR/video/video_null.c \
	$$TOP_ZBAR/window.c \
	$$TOP_ZBAR/window/window_null.c \
	$$TOP_ZBAR/zimage.c \
	$$TOP_ZBAR/ZBarScanner.cpp \

# There is no support for iconv in Android, so we bring our own as QtIconv.?pp


contains(DEFINES, USE_STATUS){
message("FROM libzbar.pro:")
include($$PRIS/status.pri)
}

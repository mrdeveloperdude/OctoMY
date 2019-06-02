ZBAR_TOP= $${SRCS}/libs/libzbar

contains(DEFINES, OC_USE_LIB_ZBAR){

HEADERS	+= \
	$$ZBAR_TOP/zbar/c++/Decoder.h
	$$ZBAR_TOP/zbar/c++/Exception.h
	$$ZBAR_TOP/zbar/c++/Image.hpp
	$$ZBAR_TOP/zbar/c++/ImageScanner.h
	$$ZBAR_TOP/zbar/c++/Processor.h
	$$ZBAR_TOP/zbar/c++/QZBarImage.h
	$$ZBAR_TOP/zbar/c++/Scanner.h
	$$ZBAR_TOP/zbar/c++/Symbol.h
	$$ZBAR_TOP/zbar/c++/Video.h
	$$ZBAR_TOP/zbar/c++/Window.h
	$$ZBAR_TOP/zbar/config.h
	$$ZBAR_TOP/zbar/debug.h
	$$ZBAR_TOP/zbar/decoder/code128.h
	$$ZBAR_TOP/zbar/decoder/code39.h
	$$ZBAR_TOP/zbar/decoder/ean.h
	$$ZBAR_TOP/zbar/decoder/i25.h
	$$ZBAR_TOP/zbar/decoder/qr_finder.h
	$$ZBAR_TOP/zbar/decoder.h
	$$ZBAR_TOP/zbar/event.h
	$$ZBAR_TOP/zbar/image.h
	$$ZBAR_TOP/zbar/img_scanner.h
	$$ZBAR_TOP/zbar/mutex.h
	$$ZBAR_TOP/zbar/processor/posix.h
	$$ZBAR_TOP/zbar/processor.h
	$$ZBAR_TOP/zbar/qrcode/bch15_5.h
	$$ZBAR_TOP/zbar/qrcode/binarize.h
	$$ZBAR_TOP/zbar/qrcode/isaac.h
	$$ZBAR_TOP/zbar/qrcode/qrdec.h
	$$ZBAR_TOP/zbar/qrcode/rs.h
	$$ZBAR_TOP/zbar/qrcode/util.h
	$$ZBAR_TOP/zbar/qrcode.h
	$$ZBAR_TOP/zbar/QtIconv.h
	$$ZBAR_TOP/zbar/refcnt.h
	$$ZBAR_TOP/zbar/svg.h
	$$ZBAR_TOP/zbar/symbol.h
	$$ZBAR_TOP/zbar/thread.h
	$$ZBAR_TOP/zbar/timer.h
	$$ZBAR_TOP/zbar/video.h
	$$ZBAR_TOP/zbar/window.h
	$$ZBAR_TOP/zbar/zbar.h
	$$ZBAR_TOP/zbar/zbar_error.h



SOURCES	+= \
	$$ZBAR_TOP/zbar/config.c \
	$$ZBAR_TOP/zbar/convert.c \
	$$ZBAR_TOP/zbar/decoder/code128.c \
	$$ZBAR_TOP/zbar/decoder/code39.c \
	$$ZBAR_TOP/zbar/decoder/ean.c \
	$$ZBAR_TOP/zbar/decoder/i25.c \
	$$ZBAR_TOP/zbar/decoder/qr_finder.c \
	$$ZBAR_TOP/zbar/decoder.c \
	$$ZBAR_TOP/zbar/img_scanner.c \
	$$ZBAR_TOP/zbar/processor/posix.c \
	$$ZBAR_TOP/zbar/processor/processor_null.c \
	$$ZBAR_TOP/zbar/processor.c \
	$$ZBAR_TOP/zbar/qrcode/bch15_5.c \
	$$ZBAR_TOP/zbar/qrcode/binarize.c \
	$$ZBAR_TOP/zbar/qrcode/isaac.c \
	$$ZBAR_TOP/zbar/qrcode/qrdec.c \
	$$ZBAR_TOP/zbar/qrcode/qrdectxt.c \
	$$ZBAR_TOP/zbar/qrcode/rs.c \
	$$ZBAR_TOP/zbar/qrcode/util.c \
	$$ZBAR_TOP/zbar/QtIconv.cpp \
	$$ZBAR_TOP/zbar/refcnt.c \
	$$ZBAR_TOP/zbar/scanner.c \
	$$ZBAR_TOP/zbar/svg.c \
	$$ZBAR_TOP/zbar/symbol.c \
	$$ZBAR_TOP/zbar/video/video_null.c \
	$$ZBAR_TOP/zbar/video.c \
	$$ZBAR_TOP/zbar/window/window_null.c \
	$$ZBAR_TOP/zbar/window.c \
	$$ZBAR_TOP/zbar/zbar_error.c \
	$$ZBAR_TOP/zbar/zimage.c \


RESOURCES += \

}

#NOTE: These are always included to keep the customers of this lib happy

HEADERS	+= \
	$$ZBAR_TOP/zbar/ZBarScanner.hpp

SOURCES	+= \
	$$ZBAR_TOP/zbar/ZBarScanner.cpp

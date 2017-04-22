TARGET =	web
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)


include(qhttp/qhttp.pri)
INCLUDEPATH += ./qhttp

INCLUDEPATH += ./template

SOURCES  += \
	template/Mustache.cpp \


HEADERS  += \
	template/Mustache.hpp \

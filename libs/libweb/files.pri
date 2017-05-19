
include(qhttp/qhttp.pri)

I=$$TOP_PWD/libs/libweb/template
L=$$TOP_PWD/libs/libweb/template


INCLUDEPATH += $$I

SOURCES  += \
	$$L/Mustache.cpp \


HEADERS  += \
	$$I/Mustache.hpp \


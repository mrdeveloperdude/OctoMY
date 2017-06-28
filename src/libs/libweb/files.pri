include(qhttp/qhttp.pri)

I=$$SRCS/libs/libweb/template
L=$$SRCS/libs/libweb/template


INCLUDEPATH += $$I

SOURCES  += \
	$$L/Mustache.cpp \


HEADERS  += \
	$$I/Mustache.hpp \


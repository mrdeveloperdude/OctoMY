
DEFINES  += QHTTP_MEMORY_LOG=0
win32:DEFINES += QHTTP_EXPORT

I=$$SRCS/libs/libweb/qhttp
L=$$SRCS/libs/libweb/qhttp


INCLUDEPATH += $$I
INCLUDEPATH += $$I/private


SOURCES  += \
	$$L/http_parser.c \
	$$L/qhttpabstracts.cpp \
	$$L/qhttpclient.cpp \
	$$L/qhttpclientrequest.cpp \
	$$L/qhttpclientresponse.cpp \
	$$L/qhttpserverconnection.cpp \
	$$L/qhttpserver.cpp \
	$$L/qhttpserverrequest.cpp \
	$$L/qhttpserverresponse.cpp \



HEADERS  += \
	$$I/http_parser.h \
	$$I/private/qhttpbase.hpp \
	$$I/private/qhttpclient_private.hpp \
	$$I/private/qhttpclientrequest_private.hpp \
	$$I/private/qhttpclientresponse_private.hpp \
	$$I/private/qhttpserverconnection_private.hpp \
	$$I/private/qhttpserver_private.hpp \
	$$I/private/qhttpserverrequest_private.hpp \
	$$I/private/qhttpserverresponse_private.hpp \
	$$I/qhttpabstracts.hpp \
	$$I/qhttpclient.hpp \
	$$I/qhttpclientrequest.hpp \
	$$I/qhttpclientresponse.hpp \
	$$I/qhttpfwd.hpp \
	$$I/qhttpserverconnection.hpp \
	$$I/qhttpserver.hpp \
	$$I/qhttpserverrequest.hpp \
	$$I/qhttpserverresponse.hpp \




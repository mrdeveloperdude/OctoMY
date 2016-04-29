TARGET =	qhttp
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)

INCLUDEPATH += ./

DEFINES  += QHTTP_MEMORY_LOG=0
win32:DEFINES += QHTTP_EXPORT


SOURCES  += \
	http_parser.c \
	qhttpabstracts.cpp \
	qhttpclient.cpp \
	qhttpclientrequest.cpp \
	qhttpclientresponse.cpp \
	qhttpserverconnection.cpp \
	qhttpserver.cpp \
	qhttpserverrequest.cpp \
	qhttpserverresponse.cpp \


HEADERS  += \
	http_parser.h \
	private/qhttpbase.hpp \
	private/qhttpclient_private.hpp \
	private/qhttpclientrequest_private.hpp \
	private/qhttpclientresponse_private.hpp \
	private/qhttpserverconnection_private.hpp \
	private/qhttpserver_private.hpp \
	private/qhttpserverrequest_private.hpp \
	private/qhttpserverresponse_private.hpp \
	qhttpabstracts.hpp \
	qhttpclient.hpp \
	qhttpclientrequest.hpp \
	qhttpclientresponse.hpp \
	qhttpfwd.hpp \
	qhttpserverconnection.hpp \
	qhttpserver.hpp \
	qhttpserverrequest.hpp \
	qhttpserverresponse.hpp \



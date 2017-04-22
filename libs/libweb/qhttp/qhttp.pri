
DEFINES  += QHTTP_MEMORY_LOG=0
win32:DEFINES += QHTTP_EXPORT

QHTTP_BASE=qhttp

SOURCES  += \
	$$QHTTP_BASE/http_parser.c \
	$$QHTTP_BASE/qhttpabstracts.cpp \
	$$QHTTP_BASE/qhttpclient.cpp \
	$$QHTTP_BASE/qhttpclientrequest.cpp \
	$$QHTTP_BASE/qhttpclientresponse.cpp \
	$$QHTTP_BASE/qhttpserverconnection.cpp \
	$$QHTTP_BASE/qhttpserver.cpp \
	$$QHTTP_BASE/qhttpserverrequest.cpp \
	$$QHTTP_BASE/qhttpserverresponse.cpp \



HEADERS  += \
	$$QHTTP_BASE/http_parser.h \
	$$QHTTP_BASE/private/qhttpbase.hpp \
	$$QHTTP_BASE/private/qhttpclient_private.hpp \
	$$QHTTP_BASE/private/qhttpclientrequest_private.hpp \
	$$QHTTP_BASE/private/qhttpclientresponse_private.hpp \
	$$QHTTP_BASE/private/qhttpserverconnection_private.hpp \
	$$QHTTP_BASE/private/qhttpserver_private.hpp \
	$$QHTTP_BASE/private/qhttpserverrequest_private.hpp \
	$$QHTTP_BASE/private/qhttpserverresponse_private.hpp \
	$$QHTTP_BASE/qhttpabstracts.hpp \
	$$QHTTP_BASE/qhttpclient.hpp \
	$$QHTTP_BASE/qhttpclientrequest.hpp \
	$$QHTTP_BASE/qhttpclientresponse.hpp \
	$$QHTTP_BASE/qhttpfwd.hpp \
	$$QHTTP_BASE/qhttpserverconnection.hpp \
	$$QHTTP_BASE/qhttpserver.hpp \
	$$QHTTP_BASE/qhttpserverrequest.hpp \
	$$QHTTP_BASE/qhttpserverresponse.hpp \




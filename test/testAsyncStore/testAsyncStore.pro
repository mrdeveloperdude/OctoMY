include($$TESTS/pris/test.pri)

TARGET = test_asyncstore

HEADERS += \
	TestAsyncStore.hpp \
	BackendMock.hpp \
	FrontendMock.hpp \
	AsyncStoreTester.hpp

SOURCES += \
	TestAsyncStore.cpp \
	BackendMock.cpp \
	FrontendMock.cpp \
	AsyncStoreTester.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)


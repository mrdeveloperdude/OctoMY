include($$TESTS/test.pri)

TARGET = test_commschannel

HEADERS += \
	TestCommsChannel.hpp \
    CommsSignalLogger.hpp \
    CommsTester.hpp \
    TestCourier.hpp

SOURCES += \
	TestCommsChannel.cpp \
    testMultiple.cpp \
    testSingle.cpp \
    testHandshake.cpp \
    CommsSignalLogger.cpp \
    CommsTester.cpp \
    TestCourier.cpp

include($$TESTS/test_autorun.pri)

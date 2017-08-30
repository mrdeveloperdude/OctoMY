include($$TESTS/test.pri)

TARGET = test_commschannel

HEADERS += \
	CommsSignalLogger.hpp \
	CommsTester.hpp \
	TestCommsChannel.hpp \
	TestCourier.hpp \
    MockCommsCarrierLog.hpp \
    MockCommsCarrier.hpp

SOURCES += \
	CommsSignalLogger.cpp \
	CommsTester.cpp \
	testCarrier.cpp \
	TestCommsChannel.cpp \
	TestCourier.cpp \
	testMultiple.cpp \
	testSingle.cpp \
    MockCommsCarrierLog.cpp \
    MockCommsCarrier.cpp \
    testCommsMock.cpp

include($$TESTS/test_autorun.pri)

include($$TESTS/pris/test.pri)

TARGET = test_commschannel

HEADERS += \
	CommsSignalLogger.hpp \
	CommsTester.hpp \
	TestCommsChannel.hpp \
	../common_test/mock/MockCommsCarrierLog.hpp \
	../common_test/mock/MockCommsCarrier.hpp \
	../common_test/mock/MockCourier.hpp

SOURCES += \
	TestCommsChannel.cpp \
	../common_test/mock/MockCommsCarrierLog.cpp \
	../common_test/mock/MockCommsCarrier.cpp \
	../common_test/mock/MockCourier.cpp \
	CommsSignalLogger.cpp \
	CommsTester.cpp \
	testMultiple.cpp \
	testSingle.cpp \
	testCommsMock.cpp \

include($$TESTS/test_autorun.pri)

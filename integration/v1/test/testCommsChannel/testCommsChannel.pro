include($$TESTS/test.pri)

TARGET = test_commschannel

HEADERS += \
	CommsSignalLogger.hpp \
	CommsTester.hpp \
	TestCommsChannel.hpp \
	../common/mock/MockCommsCarrierLog.hpp \
	../common/mock/MockCommsCarrier.hpp \
	../common/mock/MockCourier.hpp

SOURCES += \
	TestCommsChannel.cpp \
	../common/mock/MockCommsCarrierLog.cpp \
	../common/mock/MockCommsCarrier.cpp \
	../common/mock/MockCourier.cpp \
	CommsSignalLogger.cpp \
	CommsTester.cpp \
	testMultiple.cpp \
	testSingle.cpp \
	testCommsMock.cpp \

include($$TESTS/test_autorun.pri)

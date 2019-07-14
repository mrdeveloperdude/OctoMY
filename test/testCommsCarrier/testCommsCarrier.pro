include($$TESTS/pris/test.pri)

TARGET = test_commscarrier

HEADERS += \
	TestCommsCarrier.hpp \
	../common_test/mock/MockCommsCarrierLog.hpp \
	../common_test/mock/MockCommsCarrier.hpp \
	../common_test/mock/MockCourier.hpp \

SOURCES += \
	TestCommsCarrier.cpp \
	../common_test/mock/MockCommsCarrierLog.cpp \
	../common_test/mock/MockCommsCarrier.cpp \
	../common_test/mock/MockCourier.cpp \

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)

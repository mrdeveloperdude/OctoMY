include($$TESTS/pris/test.pri)

TARGET = test_commscarrier

HEADERS += \
	TestCommsCarrier.hpp \
	../common/mock/MockCommsCarrierLog.hpp \
	../common/mock/MockCommsCarrier.hpp

SOURCES += \
	TestCommsCarrier.cpp \
	../common/mock/MockCommsCarrierLog.cpp \
	../common/mock/MockCommsCarrier.cpp


include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)


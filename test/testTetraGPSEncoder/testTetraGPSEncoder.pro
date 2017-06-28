include($$TESTS/test.pri)

TARGET = test_tetragpsencoder

HEADERS += \
	TestTetraGPSEncoder.hpp

SOURCES += \
	TestTetraGPSEncoder.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

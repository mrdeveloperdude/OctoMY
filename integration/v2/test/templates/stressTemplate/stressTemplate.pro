include($$TESTS/pris/stress.pri)

TARGET = stress_template

HEADERS += \
	StressTemplate.hpp

SOURCES += \
	StressTemplate.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)


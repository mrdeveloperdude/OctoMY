include($$TESTS/pris/profile.pri)

TARGET = profile_template

HEADERS += \
	ProfileTemplate.hpp

SOURCES += \
	ProfileTemplate.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)


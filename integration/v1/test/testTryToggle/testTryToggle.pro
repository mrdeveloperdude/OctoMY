include($$TESTS/test.pri)

TARGET = test_trytoggle

HEADERS += \
	TestTryToggle.hpp \
    TryToggleTestUI.hpp

SOURCES += \
	TestTryToggle.cpp \
    TryToggleTestUI.cpp

include($$PRIS/status.pri)
include($$TESTS/test_autorun.pri)

FORMS += \
    TryToggleTestUI.ui


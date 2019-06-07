include($$TESTS/pris/test.pri)

TARGET = test_trytoggle

HEADERS += \
	TestTryToggle.hpp \
	TryToggleTestUI.hpp

SOURCES += \
	TestTryToggle.cpp \
	TryToggleTestUI.cpp

FORMS += \
	TryToggleTestUI.ui

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)

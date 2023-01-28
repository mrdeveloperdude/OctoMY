include($$TESTS/pris/test.pri)

TARGET = test_markdowneditor

HEADERS += \
	MarkdownTestInput.hpp \
	TestMarkdownEditor.hpp

SOURCES += \
	MarkdownTestInput.cpp \
	TestMarkdownEditor.cpp

include($$PRIS/status.pri)
include($$TESTS/pris/test_autorun.pri)


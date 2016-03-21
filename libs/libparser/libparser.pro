TARGET =	parser
TEMPLATE =	lib
CONFIG +=	staticlib
include($$TOP_PWD/common.pri)

# Include the qmake_extra_compilers for flex and qlalr
include(flex.pri)
include(qlalr.pri)


# Link to flex library
LIBS += -lfl

# Specify flex sources.
# NOTE: the basename of this file will need to match class names etc.
#       Please see extra comments inside file for more details.
FLEXSOURCES = octomy.l

# Specify qlalr sources.
# NOTE: the basename of this file will need to match class names etc.
#       Please see extra comments inside file for more details.
QLALRSOURCES = octomy.g

# All static source files
# NOTE: Please don't specify the intermediate sources here. They will be added
#       automatically by variable_out mechanism of extra qmake_extra_compilers
SOURCES += \
	octomy_table.cpp \
	octomy_lexer.cpp \
	octomy_parser.cpp \


HEADERS += \
	octomy_table_p.h \
	octomy_lexer.h \
	octomy_parser.hpp \



OTHER_FILES += \
	$$FLEXSOURCES \
	$$QLALRSOURCES \

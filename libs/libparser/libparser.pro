TARGET =	parser
TEMPLATE =	lib
CONFIG +=	staticlib
include($$TOP_PWD/common.pri)

# Include the qmake_extra_compilers for flex and qlalr

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


include(flex.pri)
include(qlalr.pri)

OTHER_FILES += \
	$$FLEXSOURCES \
	$$QLALRSOURCES \

include($$TOP_PWD/status.pri)
message("| FLEXSOURCES:    " $${FLEXSOURCES})
message("| QLALRSOURCES:   " $${QLALRSOURCES})
message("| GENERATED_HEADERS:   " $${GENERATED_HEADERS})
message("| GENERATED_SOURCES:    " $${GENERATED_SOURCES})

HEADERS += \
    Context.hpp \
    ParseTreeNode.hpp \
    ParseError.hpp \
    OctomyParseContext.hpp

SOURCES += \
    Context.cpp \
    ParseTreeNode.cpp \
    ParseError.cpp \
    OctomyParseContext.cpp


if(!android){

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

# Include the qmake_extra_compilers for flex and qlalr
include($$PRIS/flex.pri)
include($$PRIS/qlalr.pri)

OTHER_FILES += \
	$$FLEXSOURCES \
	$$QLALRSOURCES \

}

TARGET =	plan
TEMPLATE =	lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)
include($$SRCS/libs/libparser/libparser.pri)

INCLUDEPATH += $$SRCS/libs/libplan/

SOURCES += \
	plan/PlanHighlighter.cpp \
	widgets/planedit/CodeEditor.cpp \
	widgets/planedit/LineNumberArea.cpp \
	widgets/planedit/PlanEditor.cpp \

HEADERS += \
	plan/PlanHighlighter.hpp \
	widgets/planedit/CodeEditor.hpp \
	widgets/planedit/LineNumberArea.hpp \
	widgets/planedit/PlanEditor.hpp \

FORMS += \
	ui/PlanEditor.ui \


contains(DEFINES, USE_STATUS){
message("FROM libplan.pro")
include($$PRIS/status.pri)
}

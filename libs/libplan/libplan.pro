TARGET =	plan
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib.pri)
include($$TOP_PWD/libs/libparser/libparser.pri)
#QT         += core core-private gui gui-private
INCLUDEPATH += $$TOP_PWD/libs/libplan/


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
include($$TOP_PWD/status.pri)
}

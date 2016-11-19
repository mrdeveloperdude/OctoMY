TARGET =	util
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)

INCLUDEPATH += ./

HEADERS	+= \
	utility/BaseTranscode.hpp \
	utility/InfInt.hpp \
	utility/ScopedTimer.hpp \
	utility/Status.hpp \
	utility/Utility.hpp \
	utility/Standard.hpp \
    utility/ArcBall.hpp \
    utility/FPSCalculator.hpp \
    utility/GLContext.hpp \
    utility/IncludeOpenGL.hpp \
    utility/widgets/PixViewer.hpp \
    utility/GLErrors.hpp


SOURCES	+= \
	utility/BaseTranscode.cpp \
	utility/ScopedTimer.cpp \
	utility/Status.cpp \
	utility/Utility.cpp \
	utility/Standard.cpp \
    utility/ArcBall.cpp \
    utility/FPSCalculator.cpp \
    utility/GLContext.cpp \
    utility/widgets/PixViewer.cpp \
    utility/GLErrors.cpp

FORMS += \
    utility/widgets/PixViewer.ui

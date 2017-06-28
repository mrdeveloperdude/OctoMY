TARGET =	util
TEMPLATE =	lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)
#QT += widgets
INCLUDEPATH += $$SRCS/libs/libutil/


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
	utility/GLErrors.hpp \
	utility/SerialSize.hpp \
	utility/BufferHoneyPot.hpp \
	utility/CrashHelp.hpp \
	utility/IncludeOpenGLIntegration.hpp


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
	utility/GLErrors.cpp \
	utility/SerialSize.cpp \
	utility/BufferHoneyPot.cpp \
	utility/CrashHelp.cpp

FORMS += \
	utility/widgets/PixViewer.ui


contains(DEFINES, USE_STATUS){
message("FROM libutil.pro:")
include($$PRIS/status.pri)
}

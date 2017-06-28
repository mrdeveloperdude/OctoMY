TARGET =	puppet
TEMPLATE =	lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)

INCLUDEPATH += ./

includes= comms
for(i, includes) {
	INCLUDEE=$$i
	include($$PRIS/libincluder.pri)
}

SOURCES +=\
	expression/EyesWidget.cpp \
	gait/GaitController.cpp \
	gait/GaitWidget.cpp \
	gait/LimbIKWidget.cpp \
	pose/NameMapping.cpp \
	pose/Pose.cpp \
	pose/PoseMapping.cpp \
	pose/PoseSequence.cpp \
	widgets/NameMappingView.cpp \
	widgets/NameMappingWidget.cpp \
	widgets/PoseMappingView.cpp \
	widgets/PoseMappingWidget.cpp \
	widgets/PoseView.cpp \



HEADERS  += \
	expression/EyesWidget.hpp \
	gait/GaitController.hpp \
	gait/GaitWidget.hpp \
	gait/LimbIKWidget.hpp \
	pose/NameMapping.hpp \
	pose/Pose.hpp \
	pose/PoseMapping.hpp \
	pose/PoseSequence.hpp \
	widgets/NameMappingView.hpp \
	widgets/NameMappingWidget.hpp \
	widgets/PoseMappingView.hpp \
	widgets/PoseMappingWidget.hpp \
	widgets/PoseView.hpp \


FORMS += \
	ui/NameMappingWidget.ui \
	ui/PoseMappingWidget.ui \


contains(DEFINES, USE_STATUS){
message("FROM libpuppet.pro:")
include($$PRIS/status.pri)
}

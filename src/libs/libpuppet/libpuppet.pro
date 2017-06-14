TARGET =	puppet
TEMPLATE =	lib
CONFIG +=	staticlib

#QT         += core core-private gui gui-private
INCLUDEPATH += $$TOP_PWD/libs/libpuppet/

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib.pri)

includes= comms
for(i, includes) {
	INCLUDEE=$$i
	include($$TOP_PWD/libincluder.pri)
}


#include($$TOP_PWD/libs/libcomms/libcomms.pri)


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
include($$TOP_PWD/status.pri)
}

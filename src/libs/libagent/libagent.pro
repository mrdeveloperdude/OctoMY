TARGET =	agent
TEMPLATE =	lib
CONFIG +=	staticlib

#QT         += core core-private gui gui-private
INCLUDEPATH += ./

include($$PRIS/common.pri)
include($$PRIS/lib.pri)
#include($$SRCS/libs/libnode/libnode.pri)

includes= node core
for(i, includes) {
	INCLUDEE=$$i
	include($$PRIS/libincluder.pri)
}


#include($$SRCS/libs/libcore/libcore.pri)
#include($$SRCS/libs/libcomms/libcomms.pri)
#include($$SRCS/libss/all_libs.pri)

I=agent
L=agent

SOURCES +=\
	$$L/AgentConstants.cpp \
	$$L/Agent.cpp \
	$$L/AgentWindow.cpp \
	$$L/AgentConfig.cpp \
	$$L/AgentConfigStore.cpp \
	$$L/AgentMobilityType.cpp \
	$$L/CourierSet.cpp \
	$$L/AgentControls.cpp \

HEADERS  += \
	$$I/AgentConstants.hpp \
	$$I/Agent.hpp \
	$$I/AgentWindow.hpp \
	$$I/AgentConfig.hpp \
	$$I/AgentConfigStore.hpp \
	$$I/AgentMobilityType.hpp \
	$$I/CourierSet.hpp \
	$$I/AgentControls.hpp \


FORMS += \
	ui/AgentWindow.ui \

contains(DEFINES, USE_STATUS){
message("FROM libagent.pro:")
include($$PRIS/status.pri)
}

TEMPLATE = app
TARGET = agent

# AGENT TIER REPRESENTS THE ROBOT ITSELF: ANY CODE HERE IS INTENDED RUN ONBOARD THE ROBOT

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib/inc.pri)
include($$TOP_PWD/lib/ext.pri)

HEADERS += \
	AgentMain.hpp

SOURCES += \
	main.cpp \
	AgentMain.cpp

message("FROM agent.pro:")
include($$TOP_PWD/status.pri)


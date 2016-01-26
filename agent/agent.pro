TEMPLATE = app
TARGET = agent

# AGENT TIER REPRESENTS THE ROBOT ITSELF: ANY CODE HERE IS INTENDED RUN ONBOARD THE ROBOT

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib/inc.pri)
include($$TOP_PWD/lib/ext.pri)

HEADERS += \
	AgentMain.hpp \

SOURCES += \
	AgentMain.cpp \

message("FROM agent.pro:")
include($$TOP_PWD/status.pri)

DISTFILES += \
	android/AndroidManifest.xml \
	android/gradle/wrapper/gradle-wrapper.jar \
	android/gradlew \
	android/res/values/libs.xml \
	android/build.gradle \
	android/gradle/wrapper/gradle-wrapper.properties \
	android/gradlew.bat \
	android/src/org/octomy/agent/Agent.java


ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android


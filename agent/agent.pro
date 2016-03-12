TEMPLATE = app
TARGET = agent

# AGENT TIER REPRESENTS THE ROBOT ITSELF: ANY CODE HERE IS INTENDED RUN ONBOARD THE ROBOT

include($$TOP_PWD/common.pri)
include($$TOP_PWD/libs/inc.pri)
include($$TOP_PWD/libs/ext.pri)

HEADERS += \
	AgentMain.hpp \

SOURCES += \
	AgentMain.cpp \

android {

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

}

message("FROM agent.pro:")
include($$TOP_PWD/status.pri)

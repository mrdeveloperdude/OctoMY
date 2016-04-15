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

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

DISTFILES += \
	$$ANDROID_PACKAGE_SOURCE_DIR/AndroidManifest.xml \
	$$ANDROID_PACKAGE_SOURCE_DIR/build.gradle \
	$$ANDROID_PACKAGE_SOURCE_DIR/gradlew \
	$$ANDROID_PACKAGE_SOURCE_DIR/gradlew.bat \
	$$ANDROID_PACKAGE_SOURCE_DIR/gradle/wrapper/gradle-wrapper.jar \
	$$ANDROID_PACKAGE_SOURCE_DIR/gradle/wrapper/gradle-wrapper.properties \
	$$ANDROID_PACKAGE_SOURCE_DIR/res/values/libs.xml \
	$$ANDROID_PACKAGE_SOURCE_DIR/src/org/octomy/agent/Agent.java

}

message("FROM agent.pro: " $$ANDROID_PACKAGE_SOURCE_DIR)
include($$TOP_PWD/status.pri)

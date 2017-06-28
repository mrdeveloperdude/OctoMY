TEMPLATE = app
TARGET = agent

# AGENT REPRESENTS THE ROBOT ITSELF: ANY CODE LINKED INTO THIS EXECUTABLE IS INTENDED RUN ONBOARD THE ROBOT

include($$PRIS/common.pri)
include($$PRIS/app.pri)
include($$PRIS/lib_headers.pri)

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

contains(DEFINES, USE_STATUS){
message("FROM agent.pro: " $$ANDROID_PACKAGE_SOURCE_DIR)
include($$PRIS/status.pri)
}

TEMPLATE = app
TARGET = hub

# HUB TIER REPRESENTS AN OPTIONAL CENTRAL COMPONENT THAT TAKES CARE OF HEAVY LIFTING
# ON BEHALF OF THE AGENT (ROBOT) IN CASES WHERE IT IS NOT FEASIBLE TO RUN ALL THIS
# CODE DIRECTLY ON THE ROBOT.
# THIS IS USEFUL WHEN CODE IS HEAVY AND/OR YOU WANT MORE DIRECT DEBUGGING CAPABILITIES


include($$TOP_PWD/common.pri)
include($$TOP_PWD/libs/inc.pri)
include($$TOP_PWD/libs/ext.pri)

HEADERS += \
	HubMain.hpp

SOURCES += \
	HubMain.cpp




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
	$$ANDROID_PACKAGE_SOURCE_DIR/src/org/octomy/hub/Hub.java

}

message("FROM hub.pro:" $$ANDROID_PACKAGE_SOURCE_DIR)
include($$TOP_PWD/status.pri)



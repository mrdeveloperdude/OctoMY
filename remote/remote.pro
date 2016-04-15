TEMPLATE = app
TARGET = remote

# REMOTE TIER REPRESENTS THE DEVICE USED TO CONTROL THE ROBOT SUCH AS A HANHELD REMOTE CONTROL OR PHONE WITH REMOTE CONTROL APP

include($$TOP_PWD/common.pri)
include($$TOP_PWD/libs/inc.pri)
include($$TOP_PWD/libs/ext.pri)

HEADERS += \
	RemoteMain.hpp

SOURCES += \
	RemoteMain.cpp

android {

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

DISTFILES += \
	$$ANDROID_PACKAGE_SOURCE_DIR/AndroidManifest.xml \
	$$ANDROID_PACKAGE_SOURCE_DIR/gradle/wrapper/gradle-wrapper.jar \
	$$ANDROID_PACKAGE_SOURCE_DIR/gradlew \
	$$ANDROID_PACKAGE_SOURCE_DIR/res/values/libs.xml \
	$$ANDROID_PACKAGE_SOURCE_DIR/build.gradle \
	$$ANDROID_PACKAGE_SOURCE_DIR/gradle/wrapper/gradle-wrapper.properties \
	$$ANDROID_PACKAGE_SOURCE_DIR/gradlew.bat \
	$$ANDROID_PACKAGE_SOURCE_DIR/src/org/octomy/remote/Remote.java

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

}


message("FROM remote.pro:")
include($$TOP_PWD/status.pri)

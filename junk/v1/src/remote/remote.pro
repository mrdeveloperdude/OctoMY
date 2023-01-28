TEMPLATE = app
TARGET = remote

# REMOTE REPRESENTS THE DEVICE USED TO CONTROL THE ROBOT SUCH AS A HANHELD REMOTE CONTROL OR PHONE WITH REMOTE CONTROL APP

include($$PRIS/common.pri)
include($$PRIS/app.pri)
include($$PRIS/lib_headers.pri)

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


contains(DEFINES, USE_STATUS){
message("FROM remote.pro:")
include($$PRIS/status.pri)
}

# The remote represents a handheld remote control. Any code linked into this executable will be executed on your smarphone as part of the remote control app.

TEMPLATE = app
TARGET = remote

include($$PRIS/common.pri)
include($$PRIS/app.pri)
include($$PRIS/lib_headers.pri)

HEADERS += \
	RemoteMain.hpp

SOURCES += \
	RemoteMain.cpp

# We support android as a target with the following files that are spesific to the android build system (ADK/NDK)
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

contains(DEFINES, OC_USE_FEATURE_STATUS){
message("FROM remote.pro:" $$ANDROID_PACKAGE_SOURCE_DIR)
include($$PRIS/status.pri)
}

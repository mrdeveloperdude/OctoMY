TEMPLATE = app
TARGET = remote

# REMOTE TIER REPRESENTS THE DEVICE USED TO CONTROL THE ROBOT SUCH AS A HANHELD REMOTE CONTROL OR PHONE WITH REMOTE CONTROL APP

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib/inc.pri)
include($$TOP_PWD/lib/ext.pri)

HEADERS += \
	RemoteMain.hpp

SOURCES += \
	RemoteMain.cpp

message("FROM remote.pro:")
include($$TOP_PWD/status.pri)

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android


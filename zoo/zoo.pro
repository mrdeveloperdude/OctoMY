TEMPLATE = app
TARGET = zoo

# ZOO TIER REPRESENTS THE WEB COMUNITY. YES, IT IS A WEBAPP MADE IN QT!
# YOU DONT NEED THIS BUT IF YOU WANT TO START YOUR OWN ZOO YOU CAN

# Start with common for project...
include($$TOP_PWD/common.pri)
# ... but ZOO is a server app, so better start acting like one!
CONFIG += console
QT -= gui opengl widgets printsupport quick

include($$TOP_PWD/libs/inc.pri)
include($$TOP_PWD/libs/ext.pri)



HEADERS += \
	ZooMain.hpp

SOURCES += \
	ZooMain.cpp


message("FROM zoo.pro:")
include($$TOP_PWD/status.pri)



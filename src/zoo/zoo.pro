# The zoo represents the web community. Yes this is a webap made with Qt.
# You won't need this, but if you really want you can start your own zoo.
# Please note that interfering with the official instance of the OctoMY zoo
# is strongly frowned uppon by tje OctoMY community. Be nice!

TEMPLATE = app
TARGET = zoo

# Start with common for project...
include($$PRIS/common.pri)
include($$PRIS/app.pri)
include($$PRIS/lib_headers.pri)

# Zoo is a server app, it has no GUI.
CONFIG += console


HEADERS += \
	ZooMain.hpp

SOURCES += \
	ZooMain.cpp

contains(DEFINES, OC_USE_FEATURE_STATUS){
message("FROM zoo.pro:")
include($$PRIS/status.pri)
}

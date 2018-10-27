TEMPLATE = app
TARGET = web


# WEB IS A TOOL FOR GENERATING THE STATIC PARTS OF THE OFFICIAL OCTOMY WEBSITE.
# THE WEBSITE IS KEPT IN GIT AS SOURCE CODE IN MARKDOWN FORMAT AND THE OUTPUT
# IS A HIERARCHY OF STATIC HTML FILES THAT MIRROR THE SOURCES.

# THIS IS CURRENTLY NOT IN USE, BUT AS SOON AS IT IS GOOD ENOUGH TO REPLACE
# THE CURRENT GOOGLE SITE IT WILL BE.

# Start with common for project...
include($$PRIS/common.pri)
include($$PRIS/app.pri)
include($$PRIS/lib_headers.pri)

QT -= gui
CONFIG += console


SOURCES += \
		Website.cpp \
		WebsiteMain.cpp

HEADERS += \
		Website.hpp \
		WebsiteMain.hpp



contains(DEFINES, USE_STATUS){
message("FROM web.pro:")
include($$PRIS/status.pri)
}

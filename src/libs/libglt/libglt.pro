TARGET =	glt
TEMPLATE =	lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)

QT         += core core-private gui gui-private
INCLUDEPATH += $$SRCS/libs/libglt/

#CHEEKY HACK YO *<8o)
INCLUDEPATH += $$SRCS/libs/libutil/

include(files.pri)

contains(DEFINES, USE_STATUS){
message("FROM libglt.pro")
include($$PRIS/status.pri)
}




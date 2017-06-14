TARGET =	clt
TEMPLATE =	lib
CONFIG +=	staticlib

contains(DEFINES, EXTERNAL_LIB_OPENCL){

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib.pri)

QT         += core core-private gui gui-private
INCLUDEPATH += ./

include(files.pri)

#INCLUDEPATH+="./OpenCL-ICD-Loader/inc"
#LIBS+= -L./OpenCL-ICD-Loader/build/bin
INCLUDEPATH+="/usr/local/include/CL"
LIBS+= -L$$TOP_PWD/libs/libclt/OpenCL-ICD-Loader/build/bin
LIBS+= -lOpenCL
PRE_TARGETDEPS += $$TOP_PWD/libs/libclt/OpenCL-ICD-Loader/build/bin/libOpenCL.so


}

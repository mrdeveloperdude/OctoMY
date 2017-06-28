TARGET =	clt
TEMPLATE =	lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)

contains(DEFINES, EXTERNAL_LIB_OPENCL){

QT         += core-private gui-private

#CHEEKY HACK YO *<8o)
includes = util glt
for(i, includes) {
	INCLUDEE=$$i
	include($$PRIS/libincluder.pri)
}


INCLUDEPATH += ./


include(files.pri)

#INCLUDEPATH+="./OpenCL-ICD-Loader/inc"
#LIBS+= -L./OpenCL-ICD-Loader/build/bin
INCLUDEPATH+="/usr/local/include/CL"
LIBS+= -L$$SRCS/libs/libclt/OpenCL-ICD-Loader/build/bin
LIBS+= -lOpenCL
PRE_TARGETDEPS += $$SRCS/libs/libclt/OpenCL-ICD-Loader/build/bin/libOpenCL.so

contains(DEFINES, USE_STATUS){
message("FROM libclt.pro")
include($$PRIS/status.pri)
}


}
else{
error("FUCK NO CLT")
}



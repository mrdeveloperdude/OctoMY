contains(DEFINES, EXTERNAL_LIB_OPENCL){

#warning ("CLT  CLT  CLT  CLT  CLT  CLT  CLT  CLT  CLT  CLT  CLT  CLT  CLT  CLT  CLT  CLT  CLT  CLT  CLT  ")

QT         += core-private gui-private


#INCLUDEPATH+="./OpenCL-ICD-Loader/inc"
#LIBS+= -L./OpenCL-ICD-Loader/build/bin
INCLUDEPATH+="/usr/local/include/CL"
LIBS+= -L$$SRCS/libs/libclt/OpenCL-ICD-Loader/build/bin
LIBS+= -lOpenCL
PRE_TARGETDEPS += $$SRCS/libs/libclt/OpenCL-ICD-Loader/build/bin/libOpenCL.so

}
else{
	#message("NO CLT")
}

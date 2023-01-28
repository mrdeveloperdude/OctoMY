contains(DEFINES, OC_USE_LIB_EXT_ESPEAK){
warning ("ESPEAK ESPEAK ESPEAK ESPEAK ESPEAK ESPEAK ESPEAK ESPEAK ESPEAK ESPEAK EPEAK ESPEAK ESPEAK ESPEAK")

INCLUDEPATH+="/usr/include/espeak-ng"
LIBS+= -lespeak-ng
PRE_TARGETDEPS += /usr/lib/x86_64-linux-gnu/libespeak-ng.a

}
else{
	#message("NO ESPEAK")
}

include($$PRIS/status.pri)

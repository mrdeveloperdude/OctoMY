contains(DEFINES, OC_USE_LIB_ESPEAK){
#warning ("ESPEAK ESPEAK ESPEAK ESPEAK ESPEAK ESPEAK ESPEAK ESPEAK ESPEAK ESPEAK EPEAK ESPEAK ESPEAK ESPEAK")

INCLUDEPATH+="/usr/include/espeak"
LIBS+= -lespeak
PRE_TARGETDEPS += /usr/lib/libespeak.a

}
else{
	#message("NO ESPEAK")
}

include($$PRIS/status.pri)

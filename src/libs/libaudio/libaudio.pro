TARGET =	audio
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib.pri)

includes= qpolarssl sec
for(i, includes) {
	INCLUDEE=$$i
	include($$TOP_PWD/libincluder.pri)
}


#include($$TOP_PWD/libs/libsec/libsec.pri)

#QT         += core core-private gui gui-private
INCLUDEPATH += ./


SOURCES +=\
	AudioStream.cpp \
	Compressor.cpp \
	CreepyVoice.cpp \
	OneOffSpeech.cpp \
	Reverb.cpp \
	Tremolo.cpp \


HEADERS  += \
	AudioStream.hpp \
	Compressor.hpp \
	CreepyVoice.hpp \
	IAudioSource.hpp \
	OneOffSpeech.hpp \
	Reverb.hpp \
	Tremolo.hpp \



#This is used for speech output
contains(DEFINES, EXTERNAL_LIB_ESPEAK){
LIBS +=         -lespeak
INCLUDEPATH +=    /usr/include/espeak
}



contains(DEFINES, USE_STATUS){
message("FROM libaudio.pro:")
include($$TOP_PWD/status.pri)
}

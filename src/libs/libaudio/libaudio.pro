TARGET =	audio
TEMPLATE =	lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)

includes= qpolarssl sec
for(i, includes) {
	INCLUDEE=$$i
	include($$PRIS/libincluder.pri)
}

INCLUDEPATH += ./

TOP_AUDIO=audio/

SOURCES +=\
	$$TOP_AUDIO/AudioStream.cpp \
	$$TOP_AUDIO/Compressor.cpp \
	$$TOP_AUDIO/CreepyVoice.cpp \
	$$TOP_AUDIO/OneOffSpeech.cpp \
	$$TOP_AUDIO/Reverb.cpp \
	$$TOP_AUDIO/Tremolo.cpp \


HEADERS  += \
	$$TOP_AUDIO/AudioStream.hpp \
	$$TOP_AUDIO/Compressor.hpp \
	$$TOP_AUDIO/CreepyVoice.hpp \
	$$TOP_AUDIO/IAudioSource.hpp \
	$$TOP_AUDIO/OneOffSpeech.hpp \
	$$TOP_AUDIO/Reverb.hpp \
	$$TOP_AUDIO/Tremolo.hpp \


#This is used for speech output
contains(DEFINES, EXTERNAL_LIB_ESPEAK){
LIBS +=         -lespeak
INCLUDEPATH +=    /usr/include/espeak
}



contains(DEFINES, USE_STATUS){
message("FROM libaudio.pro:")
include($$PRIS/status.pri)
}

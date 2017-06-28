TARGET =	rng
TEMPLATE =	lib
CONFIG +=	staticlib

include($$PRIS/common.pri)
include($$PRIS/lib.pri)

INCLUDEPATH += ./

RNG_TOP=rng

SOURCES +=\
	$$RNG_TOP/DevRandomRNG.cpp \
	$$RNG_TOP/DevURandomRNG.cpp \
	$$RNG_TOP/EfficientPRNG.cpp \
	$$RNG_TOP/FileRandomRNG.cpp \
	$$RNG_TOP/MersennePRNG.cpp \
	$$RNG_TOP/ParkMillerPRNG.cpp \
	$$RNG_TOP/RNG.cpp \
	$$RNG_TOP/SystemPRNG.cpp \
	$$RNG_TOP/TausPRNG.cpp \


HEADERS  += \
	$$RNG_TOP/DevRandomRNG.hpp \
	$$RNG_TOP/DevURandomRNG.hpp \
	$$RNG_TOP/EfficientPRNG.hpp \
	$$RNG_TOP/FileRandomRNG.hpp \
	$$RNG_TOP/MersennePRNG.hpp \
	$$RNG_TOP/ParkMillerPRNG.hpp \
	$$RNG_TOP/RNG.hpp \
	$$RNG_TOP/SystemPRNG.hpp \
	$$RNG_TOP/TausPRNG.hpp \


contains(DEFINES, USE_STATUS){
message("FROM librng.pro:")
include($$PRIS/status.pri)
}

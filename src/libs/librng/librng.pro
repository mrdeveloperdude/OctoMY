TARGET =	rng
TEMPLATE =	lib
CONFIG +=	staticlib

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib.pri)
#QT         += core core-private gui gui-private
INCLUDEPATH += ./


SOURCES +=\
	DevRandomRNG.cpp \
	DevURandomRNG.cpp \
	EfficientPRNG.cpp \
	FileRandomRNG.cpp \
	MersennePRNG.cpp \
	ParkMillerPRNG.cpp \
	RNG.cpp \
	SystemPRNG.cpp \
	TausPRNG.cpp \




HEADERS  += \
	DevRandomRNG.hpp \
	DevURandomRNG.hpp \
	EfficientPRNG.hpp \
	FileRandomRNG.hpp \
	MersennePRNG.hpp \
	ParkMillerPRNG.hpp \
	RNG.hpp \
	SystemPRNG.hpp \
	TausPRNG.hpp \


contains(DEFINES, USE_STATUS){
message("FROM librng.pro:")
include($$TOP_PWD/status.pri)
}

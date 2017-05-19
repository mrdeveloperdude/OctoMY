include($$TOP_PWD/test/stress.pri)

TARGET = stress_qr

HEADERS += \
	StressQR.hpp

SOURCES += \
	StressQR.cpp

contains(DEFINES, USE_STATUS){
message("FROM stressQR.pro:")
include($$TOP_PWD/status.pri)
}

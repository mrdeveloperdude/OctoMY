TARGET =	comms
TEMPLATE =	lib
CONFIG +=	staticlib

#QT         += core core-private gui gui-private
INCLUDEPATH += $$TOP_PWD/libs/libcomms/

include($$TOP_PWD/common.pri)
include($$TOP_PWD/lib.pri)
includes= sec core puppet
for(i, includes) {
	INCLUDEE=$$i
	include($$TOP_PWD/libincluder.pri)
}

#include($$TOP_PWD/libs/libsec/libsec.pri)
#include($$TOP_PWD/libs/libcore/libcore.pri)
#include($$TOP_PWD/libs/libpuppet/libpuppet.pri)

L=comms
I=comms


SOURCES += \
	$$L/CommsChannel.cpp \
	$$L/CommsSession.cpp \
	$$L/CommsSessionDirectory.cpp \
	$$L/CommsSignature.cpp \
	$$L/couriers/AgentStateCourier.cpp \
	$$L/couriers/BlobChunk.cpp \
	$$L/couriers/BlobCourier.cpp \
	$$L/couriers/Blob.cpp \
	$$L/couriers/BlobFuture.cpp \
	$$L/couriers/Courier.cpp \
	$$L/couriers/CourierMandate.cpp \
	$$L/couriers/DiscoveryCourier.cpp \
	$$L/couriers/SensorsCourier.cpp \
	$$L/FlowControl.cpp \
	$$L/ISyncParameter.cpp \
	$$L/messages/AgentMode.cpp \
	$$L/messages/MessageType.cpp \
	$$L/messages/SensorsMessage.cpp \
	$$L/ReliabilitySystem.cpp \
	$$L/SyncContext.cpp \
	$$L/SyncParameter.cpp \
	$$L/NetworkAddress.cpp \
	$$L/NetworkOptimizer.cpp \


HEADERS += \
	$$I/CommsChannel.hpp \
	$$I/CommsSessionDirectory.hpp \
	$$I/CommsSession.hpp \
	$$I/CommsSignature.hpp \
	$$I/couriers/AgentStateCourier.hpp \
	$$I/couriers/BlobChunk.hpp \
	$$I/couriers/BlobCourier.hpp \
	$$I/couriers/BlobFuture.hpp \
	$$I/couriers/Blob.hpp \
	$$I/couriers/Courier.hpp \
	$$I/couriers/CourierMandate.hpp \
	$$I/couriers/DiscoveryCourier.hpp \
	$$I/couriers/SensorsCourier.hpp \
	$$I/FlowControl.hpp \
	$$I/ISyncParameter.hpp \
	$$I/messages/AgentMode.hpp \
	$$I/messages/MessageType.hpp \
	$$I/messages/SensorsMessage.hpp \
	$$I/ReliabilitySystem.hpp \
	$$I/SyncContext.hpp \
	$$I/SyncParameter.hpp \
	$$I/NetworkAddress.hpp \
	$$I/NetworkOptimizer.hpp \

contains(DEFINES, USE_STATUS){
message("FROM libcomms.pro")
include($$TOP_PWD/status.pri)
}


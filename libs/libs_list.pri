

# The libraries in this list will be built as separate projects and linked in at the end. This is suitable for 3rd party code and stand-alone external projects
AUTOLIBS += \
	parser \
	zbar \
	qpolarssl \


# The libraries in this list will be built into one big library called librest
AUTOLINKS += \
	node \
	agent \
	remote \
	hub \
	zoo \
	plan \
	map \
	qr \
	web \
	style \
	qfi \
	rng \
	audio \
	dynamics \
	puppet \
	sec \
	core \
	util \
	comms \
	ardumy \

HEADERS += \
    $$PWD/libnode/node/RemoteCourierSet.hpp \
    $$PWD/libcomms/comms/CommsSessionIDType.hpp \
    $$PWD/libcomms/comms/HandshakeState.hpp

SOURCES += \
    $$PWD/libnode/node/RemoteCourierSet.cpp \
    $$PWD/libcomms/comms/HandshakeState.cpp

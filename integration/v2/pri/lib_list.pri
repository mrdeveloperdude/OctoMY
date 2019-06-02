
# The libraries in this list will be built as separate projects and linked in at the end. This is suitable for 3rd party code and stand-alone external projects
AUTOLIBS_SEPARATE += \
	markdown \
	qpolarssl \
	mbedtls \
	parser \

#	zbar \
#	qfi \



# The libraries in this list will be built into one big library called libcombined
AUTOLIBS_COMBINED += \
	util \
	uptime \
	rng \
	style \
	glt \
	widgets \
	client \
	agentclient \
	remoteclient \
	hubclient \
	zooclient \
	qr \
	zbar \
	service \
	store \
	sec \
	id \
	pair \
	comms \
	ardumy \
	hardware \
	puppet \
	res \
#	motorics \
	video \
	audio \
	gps \
	map \
	plan \
	plot \
	node \
	app \
	agent \
	remote \
	zoo \
	web \
	debug \

#	clt \


#	hub \
#	dynamics \
#	sec \



AUTOLIBS_SEPARATE=   $$unique(AUTOLIBS_SEPARATE)
AUTOLIBS_COMBINED=   $$unique(AUTOLIBS_COMBINED)
AUTOLIBS_BOTH=       $$AUTOLIBS_SEPARATE $$AUTOLIBS_COMBINED
AUTOLIBS_BOTH=       $$unique(AUTOLIBS_BOTH)

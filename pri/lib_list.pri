
# The libraries in this list will be built as separate projects and linked in at the end. This is suitable for 3rd party code and stand-alone external projects
AUTOLIBS_SEPARATE_BASE += \
	qpolarssl \
	mbedtls \
	parser \
#	zbar \


# The libraries in this list will be built into one big library called libcombined
AUTOLIBS_COMBINED_BASE += \
	markdown \
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
	voice \
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
	hub \
	zoo \
	web \
	debug \
	clt \
#	dynamics \


AUTOLIBS_OVERRIDE=false

CONFIG(debug, (debug|release):$$AUTOLIBS_OVERRIDE){
# DEBUG BUILD KEEPS ALL LIBS SEPARATE FOR BUILD SPEED
################################################################################
################################################################################

	message("----------- AUTOLIBS SEPARATION MODE -------------------")
	AUTOLIBS_SEPARATE += $$AUTOLIBS_SEPARATE_BASE $$AUTOLIBS_COMBINED_BASE

	# The libraries in this list will be built into one big library called libcombined
	AUTOLIBS_COMBINED += \


} else {
# RELEASE BUILDS EVERYTHING IT CAN IN LIBCOMBINED FOR PERFORMANCE AND SIZE
################################################################################
################################################################################
	message("----------- AUTOLIBS COMBINATION MODE -------------------")
	AUTOLIBS_SEPARATE += $$AUTOLIBS_SEPARATE_BASE
	AUTOLIBS_COMBINED += $$AUTOLIBS_COMBINED_BASE


}



AUTOLIBS_SEPARATE=   $$unique(AUTOLIBS_SEPARATE)
AUTOLIBS_COMBINED=   $$unique(AUTOLIBS_COMBINED)
AUTOLIBS_BOTH=       $$AUTOLIBS_SEPARATE $$AUTOLIBS_COMBINED
AUTOLIBS_BOTH=       $$unique(AUTOLIBS_BOTH)

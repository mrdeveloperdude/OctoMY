

# The libraries in this list will be built as separate projects and linked in at the end. This is suitable for 3rd party code and stand-alone external projects
AUTOLIBS += \
	parser \
	zbar \
	qpolarssl \
	mbedtls \
	qfi \
	markdown \


# The libraries in this list will be built into one big library called librest
AUTOLINKS += \
	clt \
	glt \
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
	vid \
	rng \
	audio \
	motorics \
	dynamics \
	puppet \
	id \
	sec \
	comms \
	ardumy \
	core \
	util \

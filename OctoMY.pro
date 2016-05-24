TEMPLATE = subdirs

include(common.pri)

SUBDIRS += \
	libs \
	hub \
	agent \
	remote \
	zoo \


# NOTE: USE_TESTS is enabled when useful in local_override.pri
contains(DEFINES, USE_TESTS){
	# Warn tghe user that there is potential problems in build config for tests
	contains(DEFINES, USE_BASIC_TESTS | USE_WEB_TESTS){
		message(TEST PROJECT WAS ADDED TO OCTOMY BUILD)
	}
	else{
		warning(USE_TESTS defined without USE_BASIC_TESTS or USE_WEB_TESTS)
	}

	SUBDIRS += \
		test \

}


contains(DEFINES, USE_DOCS){
	message(DOCUMENTATION PROJECT WAS ADDED TO OCTOMY BUILD)
	SUBDIRS += \
		doc \
}



# Dependency stuff adapted from https://vilimpoc.org/blog/2014/02/21/qmake-subdirs-project-automatic-dependencies/
libs.subdir=		libs
hub.subdir=			hub
agent.subdir=		agent
remote.subdir=		remote
zoo.subdir=			zoo
test.subdir=		test
doc.subdir=			doc


hub.depends=		libs
agent.depends=		libs
remote.depends=		libs
zoo.depends=		libs
test.depends=		libs
doc.depends=		libs hub remote agent test

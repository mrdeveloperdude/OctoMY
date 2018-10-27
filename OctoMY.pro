TEMPLATE = subdirs

CONFIG += ordered

message("")
message("New OctoMY™ qmake run started...")
message("________________________________")

include($$PRIS/common.pri)


# The executables
SUBDIRS += \
	$${SRCS}/libs \
	$${SRCS}/agent \
	$${SRCS}/remote \
	$${SRCS}/hub \
	$${SRCS}/zoo \
	$${SRCS}/web \


# NOTE: USE_TESTS_* is enabled when useful in local_override.pri
if( contains(DEFINES, USE_TESTS_BASIC) | contains(DEFINES, USE_TESTS_WEB) | contains(DEFINES, USE_TESTS_STRESS) | contains(DEFINES, USE_TESTS_SELECTED)  ){

	message(ONE OR MORE TEST PROJECTS WERE ADDED TO OCTOMY BUILD)

	SUBDIRS += \
		$${TESTS} \

}



contains(DEFINES, USE_DOCS){
	message(DOCUMENTATION PROJECT WAS ADDED TO OCTOMY BUILD)
	SUBDIRS += \
		$${DOCS}
}


RESOURCES += \
	$${SRCS}/libs/libcore/resources/icons.qrc



# Dependency stuff adapted from https://vilimpoc.org/blog/2014/02/21/qmake-subdirs-project-automatic-dependencies/
libs.subdir=		$${SRCS}/libs
hub.subdir=			$${SRCS}/hub
agent.subdir=		$${SRCS}/agent
remote.subdir=		$${SRCS}/remote
zoo.subdir=			$${SRCS}/zoo
web.subdir=			$${SRCS}/web
test.subdir=		$${TESTS}
doc.subdir=			$${DOCS}


hub.depends=		$${SRCS}/libs
agent.depends=		$${SRCS}/libs
remote.depends=		$${SRCS}/libs
zoo.depends=		$${SRCS}/libs
web.depends=		$${SRCS}/libs
test.depends=		$${SRCS}/libs
doc.depends=		$${SRCS}/libs $${SRCS}/hub $${SRCS}/remote $${SRCS}/agent $${TESTS}


include($$PRIS/status.pri)

OTHER_FILES+= $$files(pri/*, true)
OTHER_FILES+= $$files(integration/local/*, true)
OTHER_FILES+= $$files(integration/multidocker/*, true)
OTHER_FILES+= $$files(integration/deb/*, true)
OTHER_FILES+= $$files(integration/travis_ci/*, true)
#OTHER_FILES+= $$files(integration/docker/*, true)
#OTHER_FILES+= $$files(doc/*, true)
#OTHER_FILES+= $$files(design/*, true)

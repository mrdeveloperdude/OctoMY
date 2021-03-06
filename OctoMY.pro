TEMPLATE = subdirs

CONFIG += ordered

message(" ")
message("New OctoMY™ qmake run started...")
message("________________________________")

include($$PRIS/common.pri)


# The executables
SUBDIRS += \
	$${SRCS}/libs \
	$${SRCS}/agent \
	$${SRCS}/zoo \
	$${SRCS}/remote \
	$${SRCS}/hub \
	#$${SRCS}/web \


# NOTE: USE_TESTS_* is enabled when useful in local_override.pri
if( contains(DEFINES, OC_USE_TESTS_BASIC) | contains(DEFINES, OC_USE_TESTS_WEB) | contains(DEFINES, OC_USE_TESTS_STRESS) | contains(DEFINES, OC_USE_TESTS_SELECTED)  ){

	message(ONE OR MORE TEST PROJECTS WERE ADDED TO OCTOMY BUILD)

	SUBDIRS += \
		$${TESTS} \

}


contains(DEFINES, OC_USE_FEATURE_DOCS){
	message(DOCUMENTATION PROJECT WAS ADDED TO OCTOMY BUILD)
	SUBDIRS += \
		$${DOCS}
}


# Dependency stuff adapted from https://vilimpoc.org/blog/2014/02/21/qmake-subdirs-project-automatic-dependencies/
libs.subdir=		$${SRCS}/libs
agent.subdir=		$${SRCS}/agent
remote.subdir=		$${SRCS}/remote
hub.subdir=			$${SRCS}/hub
zoo.subdir=			$${SRCS}/zoo
web.subdir=			$${SRCS}/web
test.subdir=		$${TESTS}
doc.subdir=			$${DOCS}


agent.depends=		$${SRCS}/libs
remote.depends=		$${SRCS}/libs
hub.depends=		$${SRCS}/libs
zoo.depends=		$${SRCS}/libs
web.depends=		$${SRCS}/libs
test.depends=		$${SRCS}/libs
doc.depends=		$${SRCS}/libs $${SRCS}/hub $${SRCS}/remote $${SRCS}/agent $${TESTS}


OTHER_FILES+= $$files(pri/*, true)
OTHER_FILES+= $$files(integration/local/*, true)
OTHER_FILES+= $$files(integration/multidocker/*, true)
OTHER_FILES+= $$files(integration/deb/*, true)
OTHER_FILES+= $$files(integration/travis_ci/*, true)
OTHER_FILES+= $$files(integration/docker/*, false)
OTHER_FILES+= $$files(content/doc/*, false)
OTHER_FILES+= $$files(content/design/*, false)
OTHER_FILES+= $$files(content/website/*, false)

# While transitioning to new nodes we want to view but not compile old node sources
OTHER_FILES+= $$files(src/libs/libagent_old/*, true);
OTHER_FILES+= $$files(src/libs/libremote_old/*, true);

include($$PRIS/status.pri)

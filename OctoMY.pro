TEMPLATE = subdirs

CONFIG += ordered


# BUILD ARCHITECTURE
# AN IMPORTANT CORE VALUE OF THE OCTOMY PROJECT IS TO
# "PUT THE QUALITY OF THE END RESULT BEFORE DEVELOPER CONVENIENCE".
#
# WHAT DOES THIS MEAN IN PRACTICE ?
# IT MEANS THAT YOU AS A DEVELOPER, WHEN PRESENTED WITH A SITUATION WHERE YOU
# HAVE TO CHOOSE BETWEEN ARCHITECTURES THAT MAKES DEVELOPMENT MORE PLEASANT
# VERSUS ARCHITECTURES THAT IMPROVE QUALITY , SHOULD ALWAYS PREFER THE LATTER.
# AND IN THE CASE YOU DON'T HAVE TO CHOOSE, YOU SHOULD OPT FOR BOTH!

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


# NOTE: USE_TESTS_* is enabled when useful in local_override.pri
if( contains(DEFINES, USE_TESTS_BASIC) | contains(DEFINES, USE_TESTS_WEB) | contains(DEFINES, USE_TESTS_STRESS) | contains(DEFINES, USE_TESTS_SELECTED)  ){

	message(ONE OR MORE TEST PROJECTS WERE ADDED TO OCTOMY BUILD)

	SUBDIRS += \
		test \

}



contains(DEFINES, USE_DOCS){
	message(DOCUMENTATION PROJECT WAS ADDED TO OCTOMY BUILD)
	SUBDIRS += \
		doc \
}


RESOURCES += \
	$${SRCS}/libs/libcore/resources/icons.qrc



# Dependency stuff adapted from https://vilimpoc.org/blog/2014/02/21/qmake-subdirs-project-automatic-dependencies/
libs.subdir=		$${SRCS}/libs
hub.subdir=			$${SRCS}/hub
agent.subdir=		$${SRCS}/agent
remote.subdir=		$${SRCS}/remote
zoo.subdir=			$${SRCS}/zoo
test.subdir=		$${TESTS}
doc.subdir=			$${DOCS}


hub.depends=		$${SRCS}/libs
agent.depends=		$${SRCS}/libs
remote.depends=		$${SRCS}/libs
zoo.depends=		$${SRCS}/libs
test.depends=		$${SRCS}/libs
doc.depends=		$${SRCS}/libs $${SRCS}/hub $${SRCS}/remote $${SRCS}/agent $${TESTS}




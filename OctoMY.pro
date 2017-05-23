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

include(common.pri)

# The executables
SUBDIRS += \
	libs \
	agent \
	remote \
	hub \
	zoo \



# NOTE: USE_TESTS is enabled when useful in local_override.pri
contains(DEFINES, USE_TESTS){
	# Warn the user that there is potential problems in build config for tests
	if( contains(DEFINES, USE_BASIC_TESTS) | contains(DEFINES, USE_WEB_TESTS)  ){
		message(TEST PROJECT WAS ADDED TO OCTOMY BUILD)
	}
	else{
		error(USE_TESTS defined without USE_BASIC_TESTS or USE_WEB_TESTS)
	}

	SUBDIRS += \
		test \

}


contains(DEFINES, USE_DOCS){
	message(DOCUMENTATION PROJECT WAS ADDED TO OCTOMY BUILD)
	SUBDIRS += \
		doc \
}


RESOURCES += \
	libs/libcore/resources/icons.qrc



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




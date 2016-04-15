TEMPLATE = subdirs

include(common.pri)

SUBDIRS += \
	libs \
	hub \
	agent \
	remote \


contains(DEFINES, USE_TEST){
SUBDIRS += \
	test \
}


contains(DEFINES, USE_DOCS){
SUBDIRS += \
	doc \
}



# Dependency stuff adapted from https://vilimpoc.org/blog/2014/02/21/qmake-subdirs-project-automatic-dependencies/
libs.subdir=		libs
hub.subdir=			hub
agent.subdir=		agent
remote.subdir=		remote
test.subdir=		test
doc.subdir=			doc


hub.depends=		libs
agent.depends=		libs
remote.depends=		libs
test.depends=		libs
doc.depends=		libs hub remote agent test

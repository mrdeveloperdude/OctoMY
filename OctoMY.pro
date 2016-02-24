TEMPLATE = subdirs


SUBDIRS += \
	lib \
	hub \
	agent \
	remote \
	test \
	doc \


# Dependency stuff adapted from https://vilimpoc.org/blog/2014/02/21/qmake-subdirs-project-automatic-dependencies/
lib.subdir   = lib
hub.subdir   = hub
agent.subdir   = agent
remote.subdir   = remote
test.subdir   = test
doc.subdir   = doc

hub.depends = lib
agent.depends = lib
remote.depends = lib
test.depends = lib
doc.depends = lib hub remote agent test


QMAKE_TARGET_COMPANY = "OctoMY™"
QMAKE_TARGET_COPYRIGHT = "Copyright © 2013-2016 Lennart Rolland <lennartrolland@gmail.com>"
QMAKE_TARGET_DESCRIPTION = "N-Limbed madness™"

TEMPLATE = subdirs

SUBDIRS += \
	lib\
	server\
	agent\
	remote\
	test


# Dependency stuff adapted from https://vilimpoc.org/blog/2014/02/21/qmake-subdirs-project-automatic-dependencies/
lib.subdir   = lib
server.subdir   = server
agent.subdir   = agent
remote.subdir   = remote
test.subdir   = test

server.depends = lib
agent.depends = lib
remote.depends = lib
test.depends = lib


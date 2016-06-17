TEMPLATE = subdirs
include(test.pri)

# NOTE: USE_TESTS is enabled when useful in local_override.pri
#       and this file will be included in build only when it is
#       enabled


# NOTE: USE_BASIC_TESTS is enabled when useful in local_override.pri
contains(DEFINES, USE_BASIC_TESTS){

TEST_PROJECTS+= \
	testCommsChannel \
	testIdenticon \
	testParser \
	testZooComms \
	testHashstore \
	testTetraGPSEncoder \


STRESS_PROJECTS+= \

}

# NOTE: USE_WEB_TESTS is enabled when useful in local_override.pri
contains(DEFINES, USE_WEB_TESTS){
TEST_PROJECTS+= \
# TODO: Add the tests here that require web-connection to work here
}


SUBDIRS += \
		$$TEST_PROJECTS

SUBDIRS += \
		$$STRESS_PROJECTS



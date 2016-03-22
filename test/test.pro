TEMPLATE = subdirs
include(test.pri)

contains(DEFINES, USE_BASIC_TESTS){
TEST_PROJECTS+= \
	testCommsChannel \
	testIdenticon \
	testParser \

STRESS_PROJECTS+= \

}


contains(DEFINES, USE_WEB_TESTS){
TEST_PROJECTS+= \

}


# Build tests only on release to save compile time
release{
		DEFINES += USE_TESTS
}

contains(DEFINES, USE_TESTS){
		SUBDIRS += \
				$$TEST_PROJECTS

		SUBDIRS += \
				$$STRESS_PROJECTS


}


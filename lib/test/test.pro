TEMPLATE = subdirs
include(test.pri)

contains(DEFINES, USE_BASIC_TESTS){
TEST_PROJECTS+= \
		testBaseThread\
		testBundle\
		testFill \
		testNFC\
		testQuickContainer\
		testScopedTimer\
		testEligibility \

STRESS_PROJECTS+= \
		stressAddressResolver  \
		stressGetSubscriptionRequest \

}

contains(DEFINES, USE_DISPENSER_TESTS){
TEST_PROJECTS+= \
		stressAddressResolver\
		testAddressResolver\
		testAutomation\
		testAutomationWorker\
		testSnap7\

}

contains(DEFINES, USE_WEB_TESTS){
TEST_PROJECTS+= \
		testWebRequest\
		testDownloadManager\
		testGetSubscriptionRequest\

}


contains(DEFINES, EXTERNAL_LIB_NODAVE){
TEST_PROJECTS+=	\
		testS71200\
		testNoDave\
		testSocketFactory\

STRESS_PROJECTS+=	\
		stressNoDave\
		stressS71200 \
		stressSocketFactory\
		stressAddressResolver\

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


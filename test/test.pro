TEMPLATE = subdirs
TARGET = test_all

include($$PRIS/common.pri)
include($$PRIS/lib_headers.pri)
CONFIG += testcase
QT += testlib

CONFIG += ordered

TEST_PROJECTS+= \

# NOTE: USE_TESTS_BASIC is enabled in local_override.pri on the build server as default to include all the safe-to-run automatic tests
contains(DEFINES, USE_TESTS_BASIC){

	message(ADDING BASIC TESTS TO BUILD)
	#
	TEST_PROJECTS+= \
		testAddressBook \
		testAddressList \
		testAgentStateCourier \
		testArduMY \
		testAssociate \
		testAsyncStore \
		testBlob \
		testCarSteeringWidget \
		testCommsCarrier \
		testCommsChannel \
		testCommsSession \
		testConcurrentQueue \
		testDataStore \
		testDeliveryWizard \
		testDiscovery \
		testEventSourcing \
		testEyesWidget \
		testHashstore \
		testIDDuel \
		testIdenticon \
		testIrisRendrer \
		testKey \
		testKeyStore \
		testMapEditor\
		testNetworkAddress \
		testNetworkSettingsWidget\
		testParser \
		testPortableID \
		testPoseMappingWidget \
		testRateCalculator \
		testRNG \
		testServotor32Controller \
		testSimpleDataStore \
		testStreamCodec \
		testSyncParameter \
		testTaskQueue \
		testTetraGPSEncoder \
		testTryToggle \
		testUtility \
		testZooComms \



	# NOTE: EXTERNAL_LIB_OPENCL is enabled when useful in local_override.pri
	contains(DEFINES, EXTERNAL_LIB_OPENCL){
	message(ADDING 'OPENCL' TESTS TO BUILD)
	TEST_PROJECTS+= \
		testOpenCL \

	}


	# NOTE: EXTERNAL_LIB_ESPEAK is enabled when useful in local_override.pri
	contains(DEFINES, EXTERNAL_LIB_ESPEAK){
	message(ADDING 'ESPEAK' TESTS TO BUILD)
	TEST_PROJECTS+= \
		testESpeak \
		testCreepyVoice \

	}
}


# NOTE: USE_TESTS_WEB is enabled in local_override.pri when the build server is connected to the internet
contains(DEFINES, USE_TESTS_WEB){
	message(ADDING 'WEB' TESTS TO BUILD)
	TEST_PROJECTS+= \
	# TODO: Add the tests here that require web-connection to work here
}


# NOTE: USE_TESTS_SELECTED is enabled in local_override.pri to build a few tests taht you are working on (as a developer)
contains(DEFINES, USE_TESTS_SELECTED){
	message(ADDING 'SELECTED' TESTS TO BUILD)
	TEST_PROJECTS+= \
#		testOpenCL \
#		testNetworkSettingsWidget \
#		testLightWidget \
#		testDataStore \
#		testSimpleDataStore \
#		testDeliveryWizard \
#		testAssociate \
#		testDiscovery \
#		testAsyncStore \
#		testKeyStore \
#		testConcurrentQueue \
		testTaskQueue \
#		testCommsChannel \
#		testCommsCarrier \
#		testRateCalculator \
#		testMapEditor\

	STRESS_PROJECTS+= \
	#	stressCapabilities \
}

# NOTE: USE_TESTS_STRESS is enabled in local_overrides.pri when you want to build/run stress tests.
# NOTE: Stress tests are usually interactive and require manual intervention so they are not suitable for "automated" running.
contains(DEFINES, USE_TESTS_STRESS){
	message(ADDING 'STRESS' TESTS TO BUILD)

	STRESS_PROJECTS+= \
		stressQR \
		stressCapabilities \

}


SUBDIRS += \
	common \
	$$TEST_PROJECTS

SUBDIRS += \
	$$STRESS_PROJECTS



SUBDIRS=   $$unique(SUBDIRS)

message("| TEST_PROJECTS:    " $${TEST_PROJECTS})
message("| STRESS_PROJECTS:  " $${STRESS_PROJECTS})
message("| TEST_SUBDIRS:  " $${SUBDIRS})

#include($$PRIS/status.pri)


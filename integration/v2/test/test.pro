# Use qmake subdir function which treats each subdirectory specified in SUBDIRS variable as a separate project to be built
TEMPLATE = subdirs
TARGET = test_all

include($$PRIS/common.pri)
include($$PRIS/lib_headers.pri)

CONFIG += testcase
QT += testlib

CONFIG += ordered

TEST_PROJECTS+= \

# NOTE: OC_USE_TESTS_BASIC is enabled in local_override.pri on the build server as default to include all the safe-to-run automatic tests
contains(DEFINES, OC_USE_TESTS_BASIC){

	message(ADDING BASIC TESTS TO BUILD)
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
		testCreepyVoice \
		testDataStore \
		testDeliveryWizard \
		testDiscovery \
		testESpeak \
		testEventSourcing \
		testEyesWidget \
		testFivePhase \
		testHardwareWidget \
		testHashstore \
		testHoneymoonScheduler \
		testIDDuel \
		testIdenticon \
		testIrisRendrer \
		testKey \
		testKeyStore \
		testLightWidget \
		testLocalAddressList \
		testMapEditor \
		testNameGenerator \
		testNetworkAddress \
		testNetworkSettingsWidget \
		testOpenCL \
		testParser \
		testPortableID \
		testPoseMappingWidget \
		testRateCalculator \
		testRNG \
		testServiceManager \
		testServotor32Controller \
		testSimpleDataStore \
		testStreamCodec \
		testSyncParameter \
		testTaskQueue \
		testTetraGPSEncoder \
		testTrustSymbolWidget \
		testTryToggle \
		testUtility \
		testZooComms \





	# NOTE: OC_USE_LIB_EXT_OPENCL is enabled when useful in local_override.pri
	contains(DEFINES, OC_USE_LIB_EXT_OPENCL){
	message(ADDING 'OPENCL' TESTS TO BUILD)
	TEST_PROJECTS+= \
		testOpenCL \

	}


	# NOTE: OC_USE_LIB_EXT_ESPEAK is enabled when useful in local_override.pri
	contains(DEFINES, OC_USE_LIB_EXT_ESPEAK){
	message(ADDING 'ESPEAK' TESTS TO BUILD)
	TEST_PROJECTS+= \
		testESpeak \
		testCreepyVoice \

	}
}


# NOTE: OC_USE_TESTS_WEB can enabled in local_override.pri when the build server is connected to the internet
contains(DEFINES, OC_USE_TESTS_WEB){
	message(ADDING 'WEB' TESTS TO BUILD)
	TEST_PROJECTS+= \
	# TODO: Add the tests here that require web-connection to work here
}


# NOTE: OC_USE_TESTS_SELECTED can enabled in local_override.pri to build a few tests that you are working on (as a developer)
contains(DEFINES, OC_USE_TESTS_SELECTED){
	message(ADDING 'SELECTED' TESTS TO BUILD)
	TEST_PROJECTS+= \
#		testAddressBook \
#		testAddressList \
#		testAgentStateCourier \
#		testArduMY \
#		testAssociate \
#		testAsyncStore \
#		testBlob \
#		testCarSteeringWidget \
#		testCommsCarrier \
#		testCommsChannel \
#		testCommsSession \
#		testConcurrentQueue \
#		testCreepyVoice \
#		testDataStore \
#		testDeliveryWizard \
#		testDiscovery \
#		testESpeak \
#		testEventSourcing \
#		testEyesWidget \
#		testFivePhase \
#		testHardwareWidget \
#		testHashstore \
#		testHoneymoonScheduler \
#		testIDDuel \
#		testIdenticon \
#		testIrisRendrer \
#		testKey \
#		testKeyStore \
#		testLightWidget \
#		testLocalAddressList \
#		testMapEditor \
#		testNameGenerator \
#		testNetworkAddress \
#		testNetworkSettingsWidget \
		testOpenCL \
#		testParser \
#		testPortableID \
#		testPoseMappingWidget \
#		testRateCalculator \
#		testRNG \
#		testServiceManager \
#		testServotor32Controller \
#		testSimpleDataStore \
#		testStreamCodec \
#		testSyncParameter \
#		testTaskQueue \
#		testTetraGPSEncoder \
#		testTrustSymbolWidget \
#		testTryToggle \
#		testUtility \
#		testZooComms \


	STRESS_PROJECTS+= \
	#	stressCapabilities \


	PROFILE_PROJECTS+= \


}

# NOTE: OC_USE_TESTS_STRESS is enabled in local_overrides.pri when you want to build/run stress tests.
# NOTE: Stress tests are usually interactive and require manual intervention so they are not suitable for "automated" running.
contains(DEFINES, OC_USE_TESTS_STRESS){
	message(ADDING 'STRESS' TESTS TO BUILD)

	STRESS_PROJECTS+= \
		stressQR \
		stressCapabilities \

}


# NOTE: USE_TESTS_PROFILE is enabled in local_overrides.pri when you want to build/run profiling tests.
# NOTE: Profiling tests may be resource intensive may require manual intervention so don't assume they are suitable for automated running.
contains(DEFINES, USE_TESTS_PROFILE){
	message(ADDING 'PROFILE' TESTS TO BUILD)

	PROFILE_PROJECTS+= \

}


SUBDIRS += \
	common_test \
	$$TEST_PROJECTS

SUBDIRS += \
	$$STRESS_PROJECTS

SUBDIRS += \
	$$PROFILE_PROJECTS

SUBDIRS=   $$unique(SUBDIRS)

message("| TEST_PROJECTS:    " $${TEST_PROJECTS})
message("| STRESS_PROJECTS:  " $${STRESS_PROJECTS})
message("| PROFILE_PROJECTS: " $${PROFILE_PROJECTS})

message("| TEST_SUBDIRS:  " $${SUBDIRS})

#include($$PRIS/status.pri)


TEMPLATE = subdirs
include($$TESTS/test.pri)
TARGET = test_all
CONFIG += ordered


# NOTE: USE_TESTS is enabled when useful in local_override.pri
#       and this file will be included in build only when it is
#       enabled


# NOTE: USE_TESTS_BASIC is enabled when useful in local_override.pri
contains(DEFINES, USE_TESTS_BASIC){

#
TEST_PROJECTS+= \
	testCommon \
#	testAgentStateCourier \
#	testArduMY \
#	testBlob \
#	testCarSteeringWidget \
#	testCommsChannel \
#	testCommsSession \
#	testDeliveryWizard \
#	testDiscovery \
#	testEventSourcing \
#	testEyesWidget \
#	testHashstore \
#	testIdenticon \
#	testIrisRendrer \
#	testKey \
#	testKeyStore \
#	testNetworkAddress \
#	testNodeAssociate \
#	testNodeAssociateStore \
#	testParser \
#	testPortableID \
#	testPoseMappingWidget \
#	testRNG \
#	testServotor32Controller \
#	testStreamCodec \
#	testSyncParameter \
#	testTetraGPSEncoder \
#	testZooComms \



# NOTE: EXTERNAL_LIB_OPENCL is enabled when useful in local_override.pri
contains(DEFINES, EXTERNAL_LIB_OPENCL){
TEST_PROJECTS+= \
	testOpenCL \

}


# NOTE: EXTERNAL_LIB_ESPEAK is enabled when useful in local_override.pri
contains(DEFINES, EXTERNAL_LIB_ESPEAK){
TEST_PROJECTS+= \
	testESpeak \
	testCreepyVoice \

}

STRESS_PROJECTS+= \
#	stressQR \

}

}


# NOTE: USE_TESTS_WEB is enabled when useful in local_override.pri
contains(DEFINES, USE_TESTS_WEB){
TEST_PROJECTS+= \
# TODO: Add the tests here that require web-connection to work here
}


# NOTE: USE_TESTS_SELECTED is enabled when useful in local_override.pri
contains(DEFINES, USE_TESTS_SELECTED){
TEST_PROJECTS+= \
	testCommsChannel \

}


SUBDIRS += \
		$$TEST_PROJECTS

SUBDIRS += \
		$$STRESS_PROJECTS

#include($$PRIS/status.pri)


TEMPLATE = subdirs
include($$TESTS/test.pri)
TARGET = test_all
CONFIG += ordered


# NOTE: USE_TESTS is enabled when useful in local_override.pri
#       and this file will be included in build only when it is
#       enabled


# NOTE: USE_BASIC_TESTS is enabled when useful in local_override.pri
contains(DEFINES, USE_BASIC_TESTS){

#
TEST_PROJECTS+= \
	testCommon \
#	testAgentStateCourier \
#	testArduMY \
#	testBlob \
#	testCarSteeringWidget \
	testCommsChannel \
#	testCreepyVoice \
#	testDeliveryWizard \
#	testDiscovery \
#	testESpeak \
#	testEventSourcing \
#	testHashstore \
#	testIdenticon \
	testKey \
	testKeyStore \
#	testNetworkAddress \
	testNodeAssociate \
	testNodeAssociateStore \
#	testOpenCL \
#	testParser \
#	testPortableID \
#	testPoseMappingStore \
#	testPoseMappingWidget \
#	testRNG \
#	testServotor32Controller \
#	testSyncParameter \
#	testTetraGPSEncoder \
#	testZooComms \




STRESS_PROJECTS+= \
#	stressQR \

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

#include($$PRIS/status.pri)


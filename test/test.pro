TEMPLATE = subdirs
include($$TOP_PWD/test/test.pri)
TARGET = test_all
CONFIG += ordered


# NOTE: USE_TESTS is enabled when useful in local_override.pri
#       and this file will be included in build only when it is
#       enabled


# NOTE: USE_BASIC_TESTS is enabled when useful in local_override.pri
contains(DEFINES, USE_BASIC_TESTS){

#
TEST_PROJECTS+= \
#	testCommsChannel \
#	testIdenticon \
#	testParser \
#	testZooComms \
#	testHashstore \
#	testTetraGPSEncoder \
#	testRNG \
#	testKey \
#	testKeyStore \
#	testDiscovery \
#	testDeliveryWizard \
#	testNetworkAddress \
#	testNodeAssociate \
#	testNodeAssociateStore \
#	testPortableID \
#	testESpeak \
#	testBlob \
#	testEventSourcing \
	testSyncParameter \

#
STRESS_PROJECTS+= \
	stressQR\

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

include($$TOP_PWD/status.pri)


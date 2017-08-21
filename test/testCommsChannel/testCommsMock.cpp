#include "TestCommsChannel.hpp"

#include "MockCommsCarrier.hpp"


void TestCommsChannel::testCommsMock()
{
	MockCommsCarrier carrier;
	KeyStore keystore;
	NodeAssociateStore peers;
	CommsChannel comms(carrier, keystore, peers);

	NetworkAddress localAddress;
	comms.start(localAddress);

}

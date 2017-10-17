#include "LocalAddressList.hpp"

#include "utility/Utility.hpp"

LocalAddressList::LocalAddressList(quint16 port)
	: mPort(0)
	, mSelectedAddress(-1)
{
	OC_METHODGATE();
	setPort(port);
	updateAddresses();
}


void LocalAddressList::setPort(quint16 port)
{
	OC_METHODGATE();
	mPort=port;
}
quint16 LocalAddressList::port() const
{
	OC_METHODGATE();
	return mPort;
}

void LocalAddressList::setCurrent(QHostAddress address, quint16 port)
{
	OC_METHODGATE();
	if(contains(address)) {
		mSelectedAddress=indexOf(address);
	}
	setPort(port);
}

QHostAddress LocalAddressList::currentAddress() const{
	OC_METHODGATE();
	const auto sz=size();
	if( (sz > 0) && (mSelectedAddress < sz) ){
		return operator[](mSelectedAddress);
	}
	return QHostAddress();
}

void LocalAddressList::updateAddresses()
{
	OC_METHODGATE();
	QHostAddress last=currentAddress();
	clear();
	QList<QHostAddress> local=utility::allLocalNetworkAddresses();
	for(QHostAddress addr:local) {
		*this <<addr;
	}
	setCurrent(last, port());
}

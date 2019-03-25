#include "LocalAddressList.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

#include "utility/network/Network.hpp"


#include "comms/NetworkAddress.hpp"



LocalAddressList::LocalAddressList()
	: mPort(0)
	, mSelectedAddress(-1)
	, mObj(new QObject)
{
	OC_METHODGATE();

}

LocalAddressList::~LocalAddressList()
{
	OC_METHODGATE();
	delete mObj;
	mObj=nullptr;
}



void LocalAddressList::configure(quint16 port, bool startTimer)
{
	OC_METHODGATE();
	setPort(port);
	updateAddresses(false);
	// Look for changes in network config every XXX ms
	mTimer.setInterval(5000);
	// We don't need precision, slop is fine
	mTimer.setTimerType(Qt::CoarseTimer);
	if(!QObject::connect(&mTimer, &QTimer::timeout, mObj, [this]() {
	updateIfNeeded(false);
	}, OC_CONTYPE)) {
		qWarning()<<"ERROR: Could not connect ";
	}
	setTimerEnabled(startTimer);
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
	if(0!=port) {
		setPort(port);
	}
}

void LocalAddressList::setTimerEnabled(bool enabled)
{
	OC_METHODGATE();
	enabled?mTimer.start():mTimer.stop();
}

bool LocalAddressList::timerEnabled() const
{
	OC_METHODGATE();
	return mTimer.isActive();
}



NetworkAddress LocalAddressList::currentNetworkAddress() const
{
	OC_METHODGATE();
	return NetworkAddress(currentAddress(), port());
}

QHostAddress LocalAddressList::currentAddress() const
{
	OC_METHODGATE();
	const auto sz=size();
	if( (mSelectedAddress >= 0) && (mSelectedAddress < sz) ) {
		return operator[](mSelectedAddress);
	}
	return QHostAddress();
}

bool LocalAddressList::isUpdateNeeded()
{
	OC_METHODGATE();
	QList<QHostAddress> local=utility::network::allLocalNetworkAddresses();
	bool updateNeeded=false;
	if(local.size() != size()) {
		updateNeeded=true;
	}
	if(!updateNeeded) {
		for(QHostAddress addr:local) {
			if(!contains(addr)) {
				updateNeeded=true;
				break;
			}
		}
	}
	if(!updateNeeded) {
		for(QHostAddress addr:*this) {
			if(!local.contains(addr)) {
				updateNeeded=true;
				break;
			}
		}
	}
	return updateNeeded;
}

bool LocalAddressList::updateIfNeeded(bool keepCurrent)
{
	OC_METHODGATE();
	const bool updateNeeded=isUpdateNeeded();
	//qDebug()<<"LOCAL ADDRESS UPDATE NEEDED: "<<updateNeeded;
	if(updateNeeded) {
		updateAddresses(keepCurrent);
	}
	return updateNeeded;
}

// Update our knowlege of available local nettwork addresses
// Should be called uppon detection of changes in network configuration (via timer + updateIfNeeded())
// Will set the address that matches the current selection closest when parameter keepCurrent is set
// Otherwize it will set the address that matches default gateway closest
// (default gateway == the ip of the router that gives us access to the internet)
void LocalAddressList::updateAddresses(bool keepCurrent)
{
	OC_METHODGATE();
	QHostAddress last=currentAddress();
	const bool lastIsGood=!last.isNull();
	clear();
	const QHostAddress defaultGateway=utility::network::defaultGatewayAddress();
	QList<QHostAddress> local=utility::network::allLocalNetworkAddresses();
	//qDebug().noquote().nospace()<<"UPDATING LOCAL ADDRESSES: ";
	//qDebug().noquote().nospace()<<" + last: "<<last;
	//qDebug().noquote().nospace()<<" + gateway: "<<dgw;
	for(QHostAddress addr:local) {
		//qDebug().noquote().nospace()<<" + addr: "<<addr<<" (dgw closeness= "<< utility::addressCloseness(addr, dgw)<< ")";
		*this <<addr;
	}
	const QHostAddress closest=utility::network::closestAddress(*this, (keepCurrent && lastIsGood)?last:defaultGateway);
	//qDebug().noquote().nospace()<<" + closest: "<<closest<<" (keepCurrent="<< keepCurrent<<")";
	setCurrent(closest, port());
}


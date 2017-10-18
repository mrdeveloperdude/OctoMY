#include "LocalAddressList.hpp"

#include "utility/Utility.hpp"
#include "comms/NetworkAddress.hpp"



LocalAddressList::LocalAddressList(quint16 port, bool startTimer)
	: mPort(0)
	, mSelectedAddress(-1)
	, mObj(new QObject)
{
	OC_METHODGATE();
	setPort(port);
	updateAddresses();
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

LocalAddressList::~LocalAddressList()
{
	delete mObj;
	mObj=nullptr;
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
	enabled?mTimer.start():mTimer.stop();
}

bool LocalAddressList::timerEnabled() const
{
	return mTimer.isActive();
}



NetworkAddress LocalAddressList::currentNetworkAddress() const
{
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


bool LocalAddressList::updateIfNeeded(bool keepCurrent)
{
	QList<QHostAddress> local=utility::allLocalNetworkAddresses();
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
	qDebug()<<"LOCAL ADDRESS UPDATE NEEDED: "<<updateNeeded;
	if(updateNeeded) {
		updateAddresses(keepCurrent);
	}
	return updateNeeded;
}

// Update our knowlege of available local nettwork addresses
// Should be called uppon detection of changes in network configuration (via timer + updateIfNeeded())
// Will set the address that matches the current selection closest when parameter keepCurrent is set
// Otherwize it will set the address that matches default gateway closest
// (default gateway == the ip of the router that allows access to the internet)
void LocalAddressList::updateAddresses(bool keepCurrent)
{
	OC_METHODGATE();
	QHostAddress last=currentAddress();
	clear();
	const QHostAddress dgw=utility::defaultGatewayAddress();
	QList<QHostAddress> local=utility::allLocalNetworkAddresses();
	qDebug().noquote().nospace()<<"UPDATING LOCAL ADDRESSES: ";
	qDebug().noquote().nospace()<<" + last: "<<last;
	qDebug().noquote().nospace()<<" + gateway: "<<dgw;
	for(QHostAddress addr:local) {
		qDebug().noquote().nospace()<<" + addr: "<<addr;
		*this <<addr;
	}
	const QHostAddress closest=utility::closestAddress(*this, keepCurrent?last:dgw);
	qDebug().noquote().nospace()<<" + closest: "<<closest<<" (keepCurrent="<< keepCurrent<<")";
	setCurrent(closest, port());
}


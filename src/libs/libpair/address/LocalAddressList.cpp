#include "LocalAddressList.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

#include "utility/network/Network.hpp"


#include "comms/address/NetworkAddress.hpp"



LocalAddressList::LocalAddressList()
	: mPort(0)
	, mSelectedAddress(-1)
	, mObj(new QObject)
	, mConfigureHelper("LocalAddressList", true, true, false, true, false)
{
	OC_METHODGATE();
}

LocalAddressList::~LocalAddressList()
{
	OC_METHODGATE();
	delete mObj;
	mObj=nullptr;
}



void LocalAddressList::configure(quint16 port)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		setPort(port);
		updateAddresses(false);
		// Look for changes in network config every XXX ms
		mTimer.setInterval(5000);
		// We don't need precision, a sloppy timer is fine
		mTimer.setTimerType(Qt::CoarseTimer);
		if(!mTimeoutConnection){
			mTimeoutConnection = QObject::connect(&mTimer, &QTimer::timeout, mObj, [this]() {
				updateIfNeeded(false);
			}, OC_CONTYPE_NON_UNIQUE);
			if(! mTimeoutConnection){
				qWarning()<<"ERROR: Could not connect ";
			}	
		}else{
			qWarning()<<"ERROR: Already connected";
		}
	}
}

void LocalAddressList::activate(const bool on)
{
	OC_METHODGATE();
	if(mConfigureHelper.activate(on)) {
		on?mTimer.start():mTimer.stop();
	}
}


bool LocalAddressList::isActivated() const
{
	OC_METHODGATE();
	return mConfigureHelper.isActivated();
}


void LocalAddressList::setPort(quint16 port)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		mPort=port;
	}
}
quint16 LocalAddressList::port() const
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		return mPort;
	}
	return 0;
}

void LocalAddressList::setCurrent(QHostAddress address, quint16 port)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(contains(address)) {
			mSelectedAddress=indexOf(address);
		}
		if(0!=port) {
			setPort(port);
		}
	}
}



NetworkAddress LocalAddressList::currentNetworkAddress() const
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		return NetworkAddress(currentAddress(), port());
	}
	return NetworkAddress();
}

QHostAddress LocalAddressList::currentAddress() const
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		const auto sz=size();
		if( (mSelectedAddress >= 0) && (mSelectedAddress < sz) ) {
			return operator[](mSelectedAddress);
		}
	}
	return QHostAddress();
}

bool LocalAddressList::isUpdateNeeded()
{
	OC_METHODGATE();
	bool updateNeeded=false;
	if(mConfigureHelper.isConfiguredAsExpected()) {
		QList<QHostAddress> local=utility::network::allLocalNetworkAddresses();
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
	}
	return updateNeeded;
}

bool LocalAddressList::updateIfNeeded(bool keepCurrent)
{
	OC_METHODGATE();
	bool updateNeeded=false;
	if(mConfigureHelper.isConfiguredAsExpected()) {
		updateNeeded=isUpdateNeeded();
		//qDebug()<<"LOCAL ADDRESS UPDATE NEEDED: "<<updateNeeded;
		if(updateNeeded) {
			updateAddresses(keepCurrent);
		}
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
	if(mConfigureHelper.isConfiguredAsExpected()) {
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
}




QString LocalAddressList::toString()
{
	OC_METHODGATE();
	QString ret("LocalAddressList{");
	if(mConfigureHelper.isConfiguredAsExpected()) {
		bool first=true;
		for(QHostAddress addr:*this) {
			if(first) {
				first=false;
			} else {
				ret+=", ";
			}
			ret+=addr.toString();
		}
	} else {
		ret+="NOT CONFIGURED";
	}
	ret+="}";
	return ret;
}


QMap<QString, QString> LocalAddressList::toMap()
{
	OC_METHODGATE();
	QMap<QString, QString>  map;
	if(mConfigureHelper.isConfiguredAsExpected()) {
		int i=0;
		for(QHostAddress addr:*this) {
			map[QString("address_%1").arg(i++)]=addr.toString();
		}
	} else {
		map[QString("NOT CONFIGURED")]="NOT CONFIGURED";
	}
	return map;
}

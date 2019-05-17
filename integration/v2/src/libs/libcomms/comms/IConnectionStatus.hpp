#ifndef IONLINESTATUS_HPP
#define IONLINESTATUS_HPP


class IConnectionStatus
{
public:
	virtual ~IConnectionStatus() {}

	// IConnectionStatus interface
public:
	// Regardless of current online status, do we want to be online?
	virtual bool needsConnection()=0;
	// Regardless of current online status, we (do not) want to be online
	virtual void setNeedsConnection(bool)=0;

	// Regardless of our need to be online, are we actually online?
	virtual bool isConnected()=0;
	// Regardless of our need to be online, set us to (not) be online.
	virtual void setConnected(bool)=0;

};

#endif // IONLINESTATUS_HPP

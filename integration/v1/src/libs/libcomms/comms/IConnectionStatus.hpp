#ifndef IONLINESTATUS_HPP
#define IONLINESTATUS_HPP


class IConnectionStatus
{
public:
	IConnectionStatus();


	// IConnectionStatus interface
public:
	// We want to go online. We might not be online and we might
	virtual bool needsConnection()=0;
	virtual void setNeedsConnection(bool)=0;

	// We are currently online. We might not need to be online
	virtual bool isConnected()=0;
	virtual void setConnected(bool)=0;

};

#endif // IONLINESTATUS_HPP

#ifndef IAPPLAUNCHER_HPP
#define IAPPLAUNCHER_HPP

#include "uptime/SharedPointerWrapper.hpp"

class AppContext;

class IAppLauncher
{

public:
	virtual ~IAppLauncher()=default;

	// IAppLauncher interface
public:
	// Called when app execution is done
	virtual void appActivateDone(const bool on) =0;

	// Provide application context
	virtual QSharedPointer<AppContext> context() =0;

};

#endif
// IAPPLAUNCHER_HPP

#ifndef IAPPLAUNCHER_HPP
#define IAPPLAUNCHER_HPP


class IAppLauncher
{
public:

	virtual ~IAppLauncher()=default;
	// IAppLauncher interface

	// Called when app execution is done
	virtual void appDeInitDone() =0;
};

#endif
// IAPPLAUNCHER_HPP

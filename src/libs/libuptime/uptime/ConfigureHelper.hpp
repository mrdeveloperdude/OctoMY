#ifndef CONFIGUREHELPER_HPP
#define CONFIGUREHELPER_HPP



// Conventions

// If a member is to be shared outside the class it cannot be RIIA, but instead should be QSharedPointer<>()
// RIIA should happen in constructor initilization list if possible
// We keep the parameter list of constructors empty, and defer any passing of parameters to the configure() method
// If class is used as QSharedPointer it should NOT accept QObject *parent as parameter in constructor to prevent un-intentional double deletes from happening.

// All non-trivial classes should have a method called configure() that takes parameters and initializes the object for use (what might previously be run from constructor)
// Some classes may need to separate the configuration from the initialization. For these classes a second method activate() should be specified
// activate(bool on) should be called to switch on or off the main function of the object. That includes connecting/disconnecting signal handlers, starting/stopping timers etc.

// configure(XXX) should be called before any other methods on an object, including activate().
// Calling configure() the second time can be allowed for some classes. In that case it should reset the state that was previously set cleanly.
// If calling configure() a second time is not allowed, an error must be produced to avoid it from happening accidentally

// Common order of events: ctor() configure(XXX, YYY) activate(true) [...object is used...] activate(false) dtor()

// What is real difference between configure() and activate()?
// 1. configure() is related to allocation of memory, activate() is related to allocation of compute resources
// 1. configure() is related to one-time initialization, acticvate() is related to reconfiguration for multiple use


#include <QString>


/**
 * @brief The ConfigureHelper class is a convenience helper to simplify the
 * standard convention for calling configure() and activate() on classes that
 * implement those methods by maintaining state to remember when configure
 * and activate has been called previously, and reporting and logging any
 * deviation from the convention.
 */
class ConfigureHelper
{
private:
	const QString mName;
	const bool mNeedsConfigure;
	const bool mNeedsActivate;
	const bool mHasMultiConfigure;
	const bool mLogWarnings;
	const bool mLogChanges;
	bool mIsConfigured;
	bool mIsActive;
	bool mIsConstructed;
	bool mDidConfigureFail;
	QString mConfigureFailureReason;
public:
	explicit ConfigureHelper(
		const QString name
		, const bool needsConfigure=true
		, const bool needsActivate=false
		, const bool hasMultiConfigure=false
		, const bool logWarnings=true
		, const bool logChanges=false);

	virtual ~ConfigureHelper();

	// Passive test for configured and active
public:
	bool isConfigured()  const;
	bool isActivated()  const;
	bool didConfigureFail()  const;
	QString configureFailureReason()  const;

	// Expecting test for configured and active
public:
	bool expectConstructed(const QString &optionalLocator = QString()) const;
	bool isConfiguredAsExpected(const QString &optionalLocator = QString()) const;
	bool isActivatedAsExpected(const QString &optionalLocator = QString())  const;

public:
	// Check and return if a configuration change is ok under current state. If it is ok, also changes configured state to true
	bool configure();
	// Check and return if an activation change is ok under current state. If it is ok, also changes activation state to parameter passed
	bool activate(const bool on);
	// Explicitly notify helper that the configuration failed supplying the reason as a string
	void configureFailed(QString reason);
};

#endif
// CONFIGUREHELPER_HPP

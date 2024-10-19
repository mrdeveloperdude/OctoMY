#include "ConfigureHelper.hpp"

#include "uptime/MethodGate.hpp"

#include <QDebug>

ConfigureHelper::ConfigureHelper(
	const QString name
	, const bool needsConfigure
	, const bool needsActivate
	, const bool hasMultiConfigure
	, const bool logWarnings
	, const bool logChanges )
	: mName(name)
	, mNeedsConfigure(needsConfigure)
	, mNeedsActivate(needsActivate)
	, mHasMultiConfigure(hasMultiConfigure)
	, mLogWarnings(logWarnings)
	, mLogChanges(logChanges)
	, mIsConfigured(false)
	, mIsActive(false)
	, mIsConstructed(true)
	, mDidConfigureFail(false)
{
	OC_METHODGATE();
	if(mLogChanges) {
		qDebug() << "NOTE: " << mName << " was created";
	}
}


ConfigureHelper::~ConfigureHelper()
{
	OC_METHODGATE();
	if(!mIsConstructed) {
		qWarning() << "ERROR: " << mName << " was destructed while not constructed.";
	}
	if(isActivated()) {
		if(mLogWarnings) {
			qWarning() << "WARNING: " << mName << " was destructed while active.";
		}
	}
	if(mLogChanges) {
		qDebug() << "NOTE: " << mName << " was destructed";
	}
}


bool ConfigureHelper::isConfigured() const
{
	OC_METHODGATE();
	return mIsConfigured;
}


bool ConfigureHelper::isActivated() const
{
	OC_METHODGATE();
	return mIsActive;
}


bool ConfigureHelper::didConfigureFail()  const
{
	OC_METHODGATE();
	return mDidConfigureFail;
}


QString ConfigureHelper::configureFailureReason()  const
{
	OC_METHODGATE();
	return mConfigureFailureReason;
}


bool ConfigureHelper::expectConstructed(const QString &optionalLocator) const{
	OC_METHODGATE();
	if(!mIsConstructed) {
		qWarning() << "ERROR: " << mName << " was not constructed when expected." << optionalLocator;
		return false;
	}
	return true;
}

bool ConfigureHelper::isConfiguredAsExpected(const QString &optionalLocator) const
{
	OC_METHODGATE();
	if(!expectConstructed(optionalLocator)) {
		return false;
	}
	if(!mIsConfigured) {
		if(mLogWarnings) {
			qWarning() << "WARNING: " << mName << " was not configured when expected." << optionalLocator;
		}
	}
	return mIsConfigured;
}


bool ConfigureHelper::isActivatedAsExpected(const QString &optionalLocator)  const
{
	OC_METHODGATE();
	if(!expectConstructed(optionalLocator)) {
		return false;
	}
	if(!mIsActive) {
		if(mLogWarnings) {
			qWarning() << "WARNING: " << mName << " was not active when expected." << optionalLocator;
		}
	}
	return mIsActive;
}


bool ConfigureHelper::configure()
{
	OC_METHODGATE();
	if(!expectConstructed()) {
		return false;
	}
	if(mLogChanges) {
		if(mIsConfigured) {
			qDebug() << "NOTE: " << mName << " was multi-configured";
		} else {
			qDebug() << "NOTE: " << mName << " was configured";
		}
	}
	bool ok = false;
	// Require that we have configuration at all
	if(mNeedsConfigure) {
		if(mIsConfigured) {
			ok = mHasMultiConfigure;
			if(!ok) {
				if(mLogWarnings) {
					qWarning() << "WARNING: " << mName << " was configured more than once when not set to be multiconfigure.";
				}
			}
		} else {
			ok = true;
		}
	} else {
		if(mLogWarnings) {
			qWarning() << "WARNING: " << mName << " was configured when not set to need configuration.";
		}
	}
	if(ok) {
		mIsConfigured = true;
	}
	return ok;
}


bool ConfigureHelper::activate(const bool wantActive)
{
	OC_METHODGATE();
	if(!expectConstructed()) {
		return false;
	}	
	if(mLogChanges) {
		qDebug() << "NOTE: " << mName << " activation went from " << mIsActive << " to " << wantActive;
	}
	bool ok=false;
	// Require that we have activation at all
	if(mNeedsActivate) {
		// Require deactivation before activation
		if(mIsActive != wantActive) {
			// Require configuration if enabled
			if(mNeedsConfigure == mIsConfigured) {
				ok=true;
			} else {
				if(mLogWarnings) {
					qWarning() << "WARNING: " << mName << " had activation changed without being configured first when set to need configuration.";
				}
			}
		} else {
			if(mLogWarnings) {
				qWarning().noquote().nospace() << "WARNING: " << mName << " had activation set to " << wantActive << " twice.";
			}
		}
	} else {
		if(mLogWarnings) {
			qWarning() << "WARNING: " << mName << " had activation changed when not set to need activation.";
		}
	}
	if(ok) {
		mIsActive=wantActive;
	}
	return ok;
}


void ConfigureHelper::configureFailed(QString reason)
{
	OC_METHODGATE();
	mIsConfigured = false;
	mDidConfigureFail = true;
	mConfigureFailureReason = reason;
}

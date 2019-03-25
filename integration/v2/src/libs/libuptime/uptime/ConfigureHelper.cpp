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
{
	OC_METHODGATE();
	if(mLogChanges) {
		qDebug()<<"NOTE: "<<mName<<" was created";
	}
}


ConfigureHelper::~ConfigureHelper()
{
	OC_METHODGATE();
	if(!mIsConstructed){
		qWarning()<<"ERROR: "<<mName<<" was destructed while not constructed.";
	}
	if(isActivated()) {
		if(mLogWarnings) {
			qWarning()<<"WARNING: "<<mName<<" was destructed while active.";
		}
	}
	if(mLogChanges) {
		qDebug()<<"NOTE: "<<mName<<" was destructed";
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

bool ConfigureHelper::isConfiguredAsExpected() const
{
	OC_METHODGATE();
	if(!mIsConfigured) {
		if(mLogWarnings) {
			qWarning()<<"WARNING: "<<mName<<" was not configured when expected.";
		}
	}
	return mIsConfigured;
}

bool ConfigureHelper::isActivatedAsExpected()  const
{
	OC_METHODGATE();
	if(!mIsActive) {
		if(mLogWarnings) {
			qWarning()<<"WARNING: "<<mName<<" was not active when expected.";
		}
	}
	return mIsActive;
}

bool ConfigureHelper::configure()
{
	OC_METHODGATE();
	if(mLogChanges) {
		if(mIsConfigured) {
			qDebug()<<"NOTE: "<<mName<<" was multi-configured";
		} else {
			qDebug()<<"NOTE: "<<mName<<" was configured";
		}
	}
	bool ok=false;
	// Require that we have configuration at all
	if(mNeedsConfigure) {
		if(mIsConfigured) {
			ok=mHasMultiConfigure;
			if(!ok) {
				if(mLogWarnings) {
					qWarning()<<"WARNING: "<<mName<<" was configured more than once when not set to be multiconfigure.";
				}
			}
		} else {
			ok=true;
		}
	} else {
		if(mLogWarnings) {
			qWarning()<<"WARNING: "<<mName<<" was configured when not set to need configuration.";
		}
	}
	if(ok) {
		mIsConfigured=true;
	}
	return ok;
}

bool ConfigureHelper::activate(const bool on)
{
	OC_METHODGATE();
	if(mLogChanges) {
		qDebug()<<"NOTE: "<<mName<<" activation went from "<<mIsActive<< " to "<<on;
	}
	bool ok=false;
	// Require that we have activation at all
	if(mNeedsActivate) {
		// Require deactivation before activation
		if(mIsActive != on) {
			// Require configuration if enabled
			if(mNeedsConfigure == mIsConfigured) {
				ok=true;
			} else {
				if(mLogWarnings) {
					qWarning()<<"WARNING: "<<mName<<" had activation changed without being configured first when set to need configuration.";
				}
			}
		} else {
			if(mLogWarnings) {
				qWarning()<<"WARNING: "<<mName<<" had activation set to "<<on<<" twice.";
			}
		}
	} else {
		if(mLogWarnings) {
			qWarning()<<"WARNING: "<<mName<<" had activation changed when not set to need activation.";
		}
	}
	if(ok) {
		mIsActive=on;
	}
	return ok;
}

#ifndef SERVICEWRAPPER_HPP
#define SERVICEWRAPPER_HPP

#include "Service.hpp"

#include "uptime/MethodGate.hpp"


#include <QSharedPointer>
#include <QDebug>

/**
 * @brief The ServiceWrapper class is meant to make it possible to wrap any existing class as a serive unintrusively.
 * As an example ,we don't want classes such as KeyStore to be dependent on Service and ServiceManager, because they will not always be used as a service.
 * So instead of imposing a dependency, we make a class KeyStoreService which wraps around KeyStore. ServiceWrapper encapsulates the basics and
 * serves as a general base class for quickly being able to create consistent wrapper classes.
 */

template <class T>
class ServiceWrapper: public Service
{
private:
	QSharedPointer<T> mWrapee;
	bool mActivated;
public:
	explicit ServiceWrapper(QSharedPointer<T> wrapee, QString name, QStringList dependencies=QStringList());

	// Service interface.
public:
	void serviceActivateImp(bool on, ServiceActivatedCallback callBack=nullptr) Q_DECL_OVERRIDE;
	bool serviceActivatedImp() const Q_DECL_OVERRIDE;

	// ServiceWapper interface.
public:
	virtual void serviceWrapperActivate(QSharedPointer<T> wrapee, bool on, ServiceActivatedCallback callBack=nullptr) =0;

};

template <class T>
ServiceWrapper<T>::ServiceWrapper(QSharedPointer<T> wrapee, QString name, QStringList dependencies)
	: Service(name, dependencies)
	, mWrapee(wrapee)
	, mActivated(false)
{
	OC_METHODGATE();
}


template <class T>
void ServiceWrapper<T>::serviceActivateImp(bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	if(!mWrapee.isNull()) {
		serviceWrapperActivate(mWrapee, on, [this, on, callBack](bool ok) {
			// Siphon of the value of "activated"
			mActivated=ok?on:mActivated;
			if(nullptr!=callBack) {
				callBack(ok);
			}
		});
	} else {
		qWarning()<<"ERROR: Could not activate service because it was null";
	}
}

template <class T>
bool ServiceWrapper<T>::serviceActivatedImp() const
{
	OC_METHODGATE();
	return mActivated;
}


#endif
// SERVICEWRAPPER_HPP

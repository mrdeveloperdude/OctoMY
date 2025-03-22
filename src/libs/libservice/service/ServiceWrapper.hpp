#ifndef SERVICEWRAPPER_HPP
#define SERVICEWRAPPER_HPP

#include "Service.hpp"

#include "uptime/MethodGate.hpp"


#include "uptime/SharedPointerWrapper.hpp"
#include <QDebug>

/**
 * @brief The ServiceWrapper class is meant to make it possible to wrap any
 * existing class as a service unintrusively. As an example, we don't want
 * classes such as KeyStore to be dependent on Service and ServiceManager,
 * because they will not always be used as a service. So instead of imposing a
 * dependency, we make a class KeyStoreService which wraps around KeyStore.
 * ServiceWrapper encapsulates the basics and serves as a general base class for
 * quickly being able to create consistent wrapper classes.
 */

template <class T>
class ServiceWrapper: public Service
{
private:
	QSharedPointer<T> mService;

public:
	explicit ServiceWrapper(QSharedPointer<T> service, QString name, QStringList dependencies=QStringList());

public:
	QSharedPointer<T> service();

	// Service interface.
public:
	void serviceActivateImp(bool on, ServiceActivatedCallback callBack=nullptr) override;

	// ServiceWapper interface.
public:
	virtual void serviceWrapperActivate(QSharedPointer<T> wrapee, bool on, ServiceActivatedCallback callBack=nullptr) =0;

};


template <class T>
ServiceWrapper<T>::ServiceWrapper(QSharedPointer<T> service, QString name, QStringList dependencies)
	: Service(name, dependencies)
	, mService(service)

{
	OC_METHODGATE();
}


template <class T>
QSharedPointer<T> ServiceWrapper<T>::service()
{
	OC_METHODGATE();
	return mService;
}


template <class T>
void ServiceWrapper<T>::serviceActivateImp(bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	if(!mService.isNull()) {
		serviceWrapperActivate(mService, on, [callBack](bool on, bool ok) {
			// Siphon of the value of "activated"
			if(nullptr!=callBack) {
				callBack(on, ok);
			}
		});
	} else {
		qWarning()<<"ERROR: Could not activate service because it was null";
	}
}

#endif
// SERVICEWRAPPER_HPP

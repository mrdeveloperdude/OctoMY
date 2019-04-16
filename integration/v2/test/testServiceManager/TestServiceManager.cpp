#include "TestServiceManager.hpp"

#include "uptime/New.hpp"

#include "service/ServiceManager.hpp"
#include "service/Service.hpp"

class MockService: public Service
{
private:
	ServiceActivatedCallback mCallBack;
	bool mActivated;
	bool mWantActivated;
	bool mDirty;
	bool mAsync;

public:
	explicit MockService(QString name, QStringList dependencies=QStringList());
	virtual ~MockService() Q_DECL_OVERRIDE;
public:
	void serviceActivate(bool on, ServiceActivatedCallback callBack) Q_DECL_OVERRIDE;
	bool serviceActivated() Q_DECL_OVERRIDE;


public:
	bool mockSetActivated(const bool runCallback);
	void mockSetAsyncActivation(const bool on);

};


MockService::MockService(QString name, QStringList dependencies)
	: Service(name, dependencies)
	, mCallBack(nullptr)
	, mActivated(false)
	, mWantActivated(false)
	, mDirty(false)
	, mAsync(true)
{

}

MockService::~MockService()
{

}


void MockService::serviceActivate(bool on, ServiceActivatedCallback callBack)
{
	qDebug()<<"ACTIVATE("<<on<<" with async="<<mAsync<<") for "<<name();

	mWantActivated=on;
	mDirty=true;
	mCallBack=callBack;
	if(!mAsync) {
		mockSetActivated(nullptr!=callBack);
	}
}



bool MockService::serviceActivated()
{
	return mActivated;
}

// Simulate async activation
bool MockService::mockSetActivated(const bool runCallback)
{
	qDebug()<<"MOCK_SET_ACTIVATED(mWantActivated="<<mWantActivated<<", mActivated="<<mActivated<<", mDirty="<<mDirty<<", ) with runCallback="<<runCallback<<" for "<<name();
	if(mDirty) {
		mDirty=false;
		mActivated=mWantActivated;
		if(runCallback) {
			if(nullptr!=mCallBack) {
				mCallBack(mActivated);
				mCallBack=nullptr;
			} else {
				qDebug()<<"ERROR: No callback registered";
			}
		}
	}
	return mActivated;
}

//Choose between async and block
void MockService::mockSetAsyncActivation(const bool on)
{
	mAsync=on;
	qDebug()<<"MOCK_SET_ASYNC(on="<<mAsync<<" for "<<name();
}


void TestServiceManager::test()
{
	QSharedPointer<MockService> a(OC_NEW MockService("Service-A"));
	QSharedPointer<MockService> b(OC_NEW MockService("Service-B", QStringList{"Service-A"}));
	QSharedPointer<MockService> c(OC_NEW MockService("Service-C", QStringList{"Service-B", "Service-D"}));
	QSharedPointer<MockService> d(OC_NEW MockService("Service-D"));
	QSharedPointer<MockService> impostor_a(OC_NEW MockService("Service-A"));
	QSharedPointer<MockService> impostor_null;
	QSharedPointer<ServiceManager> man(OC_NEW ServiceManager());

	QCOMPARE(man->count(), 0);
	QCOMPARE(man->registered("Service-A"), false);
	QCOMPARE(man->registered(a), false);
	QCOMPARE(man->registered("Service-B"), false);
	QCOMPARE(man->registered(b), false);
	QCOMPARE(man->registered(impostor_a), false);
	QCOMPARE(man->registered(impostor_null), false);
	QCOMPARE(man->registered("Service-C"), false);
	QCOMPARE(man->registered(c), false);
	QCOMPARE(man->registered("Service-D"), false);
	QCOMPARE(man->registered(d), false);
	QCOMPARE(man->registered("nonexistent"), false);
	QCOMPARE(man->unRegisterService("nonexistent"), false);
	QCOMPARE(man->count(), 0);
	QCOMPARE(man->registered("Service-A"), false);
	QCOMPARE(man->registered(a), false);
	QCOMPARE(man->registered(impostor_a), false);
	QCOMPARE(man->registered("Service-B"), false);
	QCOMPARE(man->registered(b), false);
	QCOMPARE(man->registered("Service-C"), false);
	QCOMPARE(man->registered(c), false);
	QCOMPARE(man->registered("Service-D"), false);
	QCOMPARE(man->registered(d), false);
	QCOMPARE(man->registered("nonexistent"), false);
	QCOMPARE(man->count(), 0);

	QCOMPARE(man->registerService(a), true);
	QCOMPARE(man->count(), 1);
	QCOMPARE(man->registered("Service-A"), true);
	QCOMPARE(man->registered(a), true);
	QCOMPARE(man->registered(impostor_a), false);
	QCOMPARE(man->registered(impostor_null), false);
	QCOMPARE(man->registered("Service-B"), false);
	QCOMPARE(man->registered(b), false);
	QCOMPARE(man->registered("Service-C"), false);
	QCOMPARE(man->registered(c), false);
	QCOMPARE(man->registered("Service-D"), false);
	QCOMPARE(man->registered(d), false);
	QCOMPARE(man->unRegisterService("nonexistent"), false);
	QCOMPARE(man->count(), 1);
	QCOMPARE(man->unRegisterService(a), true);
	QCOMPARE(man->count(), 0);
	QCOMPARE(man->registered("Service-A"), false);
	QCOMPARE(man->registered(a), false);
	QCOMPARE(man->registered(impostor_a), false);
	QCOMPARE(man->registered("Service-B"), false);
	QCOMPARE(man->registered(b), false);
	QCOMPARE(man->registered("Service-C"), false);
	QCOMPARE(man->registered(c), false);
	QCOMPARE(man->registered("Service-D"), false);
	QCOMPARE(man->registered(d), false);
	QCOMPARE(man->registered("nonexistent"), false);
	QCOMPARE(man->count(), 0);

	QCOMPARE(man->unRegisterService(a), false);
	QCOMPARE(man->count(), 0);
	QCOMPARE(man->registered("Service-A"), false);
	QCOMPARE(man->registered(a), false);
	QCOMPARE(man->registered(impostor_a), false);
	QCOMPARE(man->registered(impostor_null), false);
	QCOMPARE(man->registered("Service-B"), false);
	QCOMPARE(man->registered(b), false);
	QCOMPARE(man->registered("Service-C"), false);
	QCOMPARE(man->registered(c), false);
	QCOMPARE(man->registered("Service-D"), false);
	QCOMPARE(man->registered(d), false);
	QCOMPARE(man->registered("nonexistent"), false);
	QCOMPARE(man->count(), 0);
	QCOMPARE(man->dependenciesMet("Service-A"), false);
	QCOMPARE(man->dependenciesMet(a), false);
	QCOMPARE(man->dependenciesMet("Service-B"), false);
	QCOMPARE(man->dependenciesMet(b), false);
	QCOMPARE(man->dependenciesMet("Service-C"), false);
	QCOMPARE(man->dependenciesMet(c), false);
	QCOMPARE(man->dependenciesMet("Service-D"), false);
	QCOMPARE(man->dependenciesMet(d), false);

	QCOMPARE(man->registerService(a), true);
	QCOMPARE(man->count(), 1);
	QCOMPARE(man->registered("Service-A"), true);
	QCOMPARE(man->registered(a), true);
	QCOMPARE(man->registered(impostor_a), false);
	QCOMPARE(man->registered(impostor_null), false);
	QCOMPARE(man->registered("Service-B"), false);
	QCOMPARE(man->registered(b), false);
	QCOMPARE(man->registered("Service-C"), false);
	QCOMPARE(man->registered(c), false);
	QCOMPARE(man->registered("Service-D"), false);
	QCOMPARE(man->registered(d), false);
	QCOMPARE(man->unRegisterService("nonexistent"), false);
	QCOMPARE(man->count(), 1);
	QCOMPARE(man->dependenciesMet("Service-A"), true);
	QCOMPARE(man->dependenciesMet(a), true);
	QCOMPARE(man->dependenciesMet("Service-B"), false);
	QCOMPARE(man->dependenciesMet(b), false);
	QCOMPARE(man->dependenciesMet("Service-C"), false);
	QCOMPARE(man->dependenciesMet(c), false);
	QCOMPARE(man->dependenciesMet("Service-D"), false);
	QCOMPARE(man->dependenciesMet(d), false);

	QCOMPARE(man->registerService(b), true);
	QCOMPARE(man->count(), 2);
	QCOMPARE(man->registered("Service-A"), true);
	QCOMPARE(man->registered(a), true);
	QCOMPARE(man->registered(impostor_a), false);
	QCOMPARE(man->registered(impostor_null), false);
	QCOMPARE(man->registered("Service-B"), true);
	QCOMPARE(man->registered(b), true);
	QCOMPARE(man->registered("Service-C"), false);
	QCOMPARE(man->registered(c), false);
	QCOMPARE(man->registered("Service-D"), false);
	QCOMPARE(man->registered(d), false);
	QCOMPARE(man->unRegisterService("nonexistent"), false);
	QCOMPARE(man->count(), 2);
	QCOMPARE(man->dependenciesMet("Service-A"), true);
	QCOMPARE(man->dependenciesMet(a), true);
	QCOMPARE(man->dependenciesMet("Service-B"), false);
	QCOMPARE(man->dependenciesMet(b), false);
	QCOMPARE(man->dependenciesMet("Service-C"), false);
	QCOMPARE(man->dependenciesMet(c), false);
	QCOMPARE(man->dependenciesMet("Service-D"), false);
	QCOMPARE(man->dependenciesMet(d), false);

	QCOMPARE(man->registerService(b), false);
	QCOMPARE(man->count(), 2);
	QCOMPARE(man->registered("Service-A"), true);
	QCOMPARE(man->registered(a), true);
	QCOMPARE(man->registered(impostor_a), false);
	QCOMPARE(man->registered(impostor_null), false);
	QCOMPARE(man->registered("Service-B"), true);
	QCOMPARE(man->registered(b), true);
	QCOMPARE(man->registered("Service-C"), false);
	QCOMPARE(man->registered(c), false);
	QCOMPARE(man->registered("Service-D"), false);
	QCOMPARE(man->registered(d), false);
	QCOMPARE(man->unRegisterService("nonexistent"), false);
	QCOMPARE(man->count(), 2);
	QCOMPARE(man->dependenciesMet("Service-A"), true);
	QCOMPARE(man->dependenciesMet(a), true);
	QCOMPARE(man->dependenciesMet("Service-B"), false);
	QCOMPARE(man->dependenciesMet(b), false);
	QCOMPARE(man->dependenciesMet("Service-C"), false);
	QCOMPARE(man->dependenciesMet(c), false);
	QCOMPARE(man->dependenciesMet("Service-D"), false);
	QCOMPARE(man->dependenciesMet(d), false);

	QCOMPARE(man->registerService(c), true);
	QCOMPARE(man->count(), 3);
	QCOMPARE(man->registered("Service-A"), true);
	QCOMPARE(man->registered(a), true);
	QCOMPARE(man->registered(impostor_a), false);
	QCOMPARE(man->registered(impostor_null), false);
	QCOMPARE(man->registered("Service-B"), true);
	QCOMPARE(man->registered(b), true);
	QCOMPARE(man->registered("Service-C"), true);
	QCOMPARE(man->registered(c), true);
	QCOMPARE(man->registered("Service-D"), false);
	QCOMPARE(man->registered(d), false);
	QCOMPARE(man->unRegisterService("nonexistent"), false);
	QCOMPARE(man->count(), 3);
	QCOMPARE(man->dependenciesMet("Service-A"), true);
	QCOMPARE(man->dependenciesMet(a), true);
	QCOMPARE(man->dependenciesMet("Service-B"), false);
	QCOMPARE(man->dependenciesMet(b), false);
	QCOMPARE(man->dependenciesMet("Service-C"), false);
	QCOMPARE(man->dependenciesMet(c), false);
	QCOMPARE(man->dependenciesMet("Service-D"), false);
	QCOMPARE(man->dependenciesMet(d), false);

	QCOMPARE(man->registerService(d), true);
	QCOMPARE(man->count(), 4);
	QCOMPARE(man->registered("Service-A"), true);
	QCOMPARE(man->registered(a), true);
	QCOMPARE(man->registered(impostor_a), false);
	QCOMPARE(man->registered(impostor_null), false);
	QCOMPARE(man->registered("Service-B"), true);
	QCOMPARE(man->registered(b), true);
	QCOMPARE(man->registered("Service-C"), true);
	QCOMPARE(man->registered(c), true);
	QCOMPARE(man->registered("Service-D"), true);
	QCOMPARE(man->registered(d), true);
	QCOMPARE(man->unRegisterService("nonexistent"), false);
	QCOMPARE(man->count(), 4);
	QCOMPARE(man->dependenciesMet("Service-A"), true);
	QCOMPARE(man->dependenciesMet(a), true);
	QCOMPARE(man->dependenciesMet("Service-B"), false);
	QCOMPARE(man->dependenciesMet(b), false);
	QCOMPARE(man->dependenciesMet("Service-C"), false);
	QCOMPARE(man->dependenciesMet(c), false);
	QCOMPARE(man->dependenciesMet("Service-D"), true);
	QCOMPARE(man->dependenciesMet(d), true);

	QCOMPARE(man->registerService(c), false);
	QCOMPARE(man->count(), 4);
	QCOMPARE(man->registered("Service-A"), true);
	QCOMPARE(man->registered(a), true);
	QCOMPARE(man->registered("Service-B"), true);
	QCOMPARE(man->registered(b), true);
	QCOMPARE(man->registered("Service-C"), true);
	QCOMPARE(man->registered(c), true);
	QCOMPARE(man->registered("Service-D"), true);
	QCOMPARE(man->registered(d), true);
	QCOMPARE(man->unRegisterService("nonexistent"), false);
	QCOMPARE(man->count(), 4);
	QCOMPARE(man->dependenciesMet("Service-A"), true);
	QCOMPARE(man->dependenciesMet(a), true);
	QCOMPARE(man->dependenciesMet("Service-B"), false);
	QCOMPARE(man->dependenciesMet(b), false);
	QCOMPARE(man->dependenciesMet("Service-C"), false);
	QCOMPARE(man->dependenciesMet(c), false);
	QCOMPARE(man->dependenciesMet("Service-D"), true);
	QCOMPARE(man->dependenciesMet(d), true);

	a->serviceActivate(true, [](bool ok) {
		qDebug()<<"YAY CALLBACK!"<<ok;
	});
	QCOMPARE(a->mockSetActivated(true), true);
	QCOMPARE(man->count(), 4);
	QCOMPARE(man->registered("Service-A"), true);
	QCOMPARE(man->registered(a), true);
	QCOMPARE(man->registered("Service-B"), true);
	QCOMPARE(man->registered(b), true);
	QCOMPARE(man->registered("Service-C"), true);
	QCOMPARE(man->registered(c), true);
	QCOMPARE(man->registered("Service-D"), true);
	QCOMPARE(man->registered(d), true);
	QCOMPARE(man->dependenciesMet("Service-A"), true);
	QCOMPARE(man->dependenciesMet(a), true);
	QCOMPARE(man->dependenciesMet("Service-B"), true);
	QCOMPARE(man->dependenciesMet(b), true);
	QCOMPARE(man->dependenciesMet("Service-C"), false);
	QCOMPARE(man->dependenciesMet(c), false);
	QCOMPARE(man->dependenciesMet("Service-D"), true);
	QCOMPARE(man->dependenciesMet(d), true);

	b->serviceActivate(true, [](bool ok) {
		qDebug()<<"YAY CALLBACK!"<<ok;
	});
	QCOMPARE(b->mockSetActivated(true), true);
	QCOMPARE(man->count(), 4);
	QCOMPARE(man->registered("Service-A"), true);
	QCOMPARE(man->registered(a), true);
	QCOMPARE(man->registered("Service-B"), true);
	QCOMPARE(man->registered(b), true);
	QCOMPARE(man->registered("Service-C"), true);
	QCOMPARE(man->registered(c), true);
	QCOMPARE(man->registered("Service-D"), true);
	QCOMPARE(man->registered(d), true);
	QCOMPARE(man->dependenciesMet("Service-A"), true);
	QCOMPARE(man->dependenciesMet(a), true);
	QCOMPARE(man->dependenciesMet("Service-B"), true);
	QCOMPARE(man->dependenciesMet(b), true);
	QCOMPARE(man->dependenciesMet("Service-C"), false);
	QCOMPARE(man->dependenciesMet(c), false);
	QCOMPARE(man->dependenciesMet("Service-D"), true);
	QCOMPARE(man->dependenciesMet(d), true);

	d->serviceActivate(true, [](bool ok) {
		qDebug()<<"YAY CALLBACK!"<<ok;
	});
	QCOMPARE(d->mockSetActivated(true), true);
	QCOMPARE(man->count(), 4);
	QCOMPARE(man->registered("Service-A"), true);
	QCOMPARE(man->registered(a), true);
	QCOMPARE(man->registered("Service-B"), true);
	QCOMPARE(man->registered(b), true);
	QCOMPARE(man->registered("Service-C"), true);
	QCOMPARE(man->registered(c), true);
	QCOMPARE(man->registered("Service-D"), true);
	QCOMPARE(man->registered(d), true);
	QCOMPARE(man->dependenciesMet("Service-A"), true);
	QCOMPARE(man->dependenciesMet(a), true);
	QCOMPARE(man->dependenciesMet("Service-B"), true);
	QCOMPARE(man->dependenciesMet(b), true);
	QCOMPARE(man->dependenciesMet("Service-C"), true);
	QCOMPARE(man->dependenciesMet(c), true);
	QCOMPARE(man->dependenciesMet("Service-D"), true);
	QCOMPARE(man->dependenciesMet(d), true);

	qDebug()<<"DEPENDENCIES A: "<<man->dependencies("Service-A");
	qDebug()<<"DEPENDENCIES B: "<<man->dependencies("Service-B");
	qDebug()<<"DEPENDENCIES C: "<<man->dependencies("Service-C");
	qDebug()<<"DEPENDENCIES D: "<<man->dependencies("Service-D");
	QSet<QString> expectedADependencies{};
	QSet<QString> expectedBDependencies{"Service-A"};
	QSet<QString> expectedCDependencies{"Service-B", "Service-D", "Service-A"};
	QSet<QString> expectedDDependencies{};
	QCOMPARE(man->dependencies("Service-A"), expectedADependencies);
	QCOMPARE(man->dependencies("Service-B"), expectedBDependencies);
	QCOMPARE(man->dependencies("Service-C"), expectedCDependencies);
	QCOMPARE(man->dependencies("Service-D"), expectedDDependencies);

	qDebug()<<"DEPENDENTS A: "<<man->dependents("Service-A");
	qDebug()<<"DEPENDENTS B: "<<man->dependents("Service-B");
	qDebug()<<"DEPENDENTS C: "<<man->dependents("Service-C");
	qDebug()<<"DEPENDENTS D: "<<man->dependents("Service-D");
	QSet<QString> expectedADependents{"Service-B", "Service-C"};
	QSet<QString> expectedBDependents{"Service-C"};
	QSet<QString> expectedCDependents{};
	QSet<QString> expectedDDependents{"Service-C"};
	QCOMPARE(man->dependents("Service-A"), expectedADependents);
	QCOMPARE(man->dependents("Service-B"), expectedBDependents);
	QCOMPARE(man->dependents("Service-C"), expectedCDependents);
	QCOMPARE(man->dependents("Service-D"), expectedDDependents);


	a->serviceActivate(false, [](bool ok) {
		qDebug()<<"YAY CALLBACK!"<<ok;
	});
	QCOMPARE(a->mockSetActivated(true), false);
	QCOMPARE(man->count(), 4);
	QCOMPARE(man->registered("Service-A"), true);
	QCOMPARE(man->registered(a), true);
	QCOMPARE(man->registered("Service-B"), true);
	QCOMPARE(man->registered(b), true);
	QCOMPARE(man->registered("Service-C"), true);
	QCOMPARE(man->registered(c), true);
	QCOMPARE(man->registered("Service-D"), true);
	QCOMPARE(man->registered(d), true);
	QCOMPARE(man->dependenciesMet("Service-A"), true);
	QCOMPARE(man->dependenciesMet(a), true);
	QCOMPARE(man->dependenciesMet("Service-B"), false);
	QCOMPARE(man->dependenciesMet(b), false);
	QCOMPARE(man->dependenciesMet("Service-C"), false);
	QCOMPARE(man->dependenciesMet(c), false);
	QCOMPARE(man->dependenciesMet("Service-D"), true);
	QCOMPARE(man->dependenciesMet(d), true);

	QSet<QString> setC{"Service-C"};
	QSet<QString> setA=man->metDependents("Service-A", false);
	man->activate(setA, true, [=](bool ok) {
		qDebug()<<"Totally called back: "<<ok;
		QCOMPARE(man->count(), 4);
		QCOMPARE(man->registered("Service-A"), true);
		QCOMPARE(man->registered(a), true);
		QCOMPARE(man->registered("Service-B"), true);
		QCOMPARE(man->registered(b), true);
		QCOMPARE(man->registered("Service-C"), true);
		QCOMPARE(man->registered(c), true);
		QCOMPARE(man->registered("Service-D"), true);
		QCOMPARE(man->registered(d), true);
		QCOMPARE(man->dependenciesMet("Service-A"), true);
		QCOMPARE(man->dependenciesMet(a), true);
		QCOMPARE(man->dependenciesMet("Service-B"), true);
		QCOMPARE(man->dependenciesMet(b), true);
		QCOMPARE(man->dependenciesMet("Service-C"), true);
		QCOMPARE(man->dependenciesMet(c), true);
		QCOMPARE(man->dependenciesMet("Service-D"), true);
		QCOMPARE(man->dependenciesMet(d), true);
	});

	qDebug()<<"#############################################################";
	a->serviceActivate(false, [](bool ok) {
		qDebug()<<"Deactivated A: "<<ok;
	});
	b->serviceActivate(false, [](bool ok) {
		qDebug()<<"Deactivated B: "<<ok;
	});
	c->serviceActivate(false, [](bool ok) {
		qDebug()<<"Deactivated C: "<<ok;
	});
	d->serviceActivate(false, [](bool ok) {
		qDebug()<<"Deactivated D: "<<ok;
	});

	QCOMPARE(a->mockSetActivated(true), false);
	QCOMPARE(b->mockSetActivated(true), false);
	QCOMPARE(c->mockSetActivated(true), false);
	QCOMPARE(d->mockSetActivated(true), false);

	QCOMPARE(a->serviceActivated(), false);
	QCOMPARE(b->serviceActivated(), false);
	QCOMPARE(c->serviceActivated(), false);
	QCOMPARE(d->serviceActivated(), false);

	/*
	QCOMPARE(man->activated("Service-A"), false);
	QCOMPARE(man->activated("Service-B"), false);
	QCOMPARE(man->activated("Service-C"), false);
	QCOMPARE(man->activated("Service-D"), false);
	*/

	qDebug()<<"=============================================================";
	man->dump();
	qDebug()<<"=============================================================";


	a->mockSetAsyncActivation(false);
	b->mockSetAsyncActivation(false);
	c->mockSetAsyncActivation(false);
	d->mockSetAsyncActivation(false);

	man->activate(man->all(), true, [](bool ok) {
		qDebug()<<"ALL ACTIVATED DONE: "<<ok;
	});
	qDebug()<<"=============================================================";
	man->dump();
	qDebug()<<"=============================================================";


	/*
	a->activate(false, [](bool ok) {
		qDebug()<<"Deactivated A: "<<ok;
	});
	b->activate(false, [](bool ok) {
		qDebug()<<"Deactivated B: "<<ok;
	});
	c->activate(false, [](bool ok) {
		qDebug()<<"Deactivated C: "<<ok;
	});
	d->activate(false, [](bool ok) {
		qDebug()<<"Deactivated D: "<<ok;
	});
	*/

	QCOMPARE(a->serviceActivated(), true);
	QCOMPARE(b->serviceActivated(), true);
	QCOMPARE(c->serviceActivated(), true);
	QCOMPARE(d->serviceActivated(), true);

	QCOMPARE(man->activated("Service-A"), true);
	QCOMPARE(man->activated("Service-B"), true);
	QCOMPARE(man->activated("Service-C"), true);
	QCOMPARE(man->activated("Service-D"), true);


}

OC_TEST_MAIN(test, TestServiceManager)

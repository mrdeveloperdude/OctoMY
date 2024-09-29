#include "TestServiceManager.hpp"

#include "uptime/New.hpp"

#include "service/ServiceManager.hpp"
#include "service/Service.hpp"
#include "service/ServiceLevel.hpp"
#include "service/ServiceLevelManager.hpp"


class MockService: public Service
{
private:
	quint8 pad[7];
	ServiceActivatedCallback mCallBack;

	bool mActivated;
	bool mWantActivated;
	bool mDirty;
	bool mAsync;
	quint8 pad2[4];

public:
	explicit MockService(QString name, QStringList dependencies=QStringList());
	virtual ~MockService() override;
public:
	void serviceActivateImp(bool on, ServiceActivatedCallback callBack) override;
//	bool serviceActivatedImp() const override;


public:
	bool mockSetActivated(const bool runCallback);
	void mockSetAsyncActivation(const bool on);

};


MockService::MockService(QString name, QStringList dependencies)
	: Service(name, dependencies)
	, pad{0}
	, mCallBack(nullptr)
	, mActivated(false)
	, mWantActivated(false)
	, mDirty(false)
	, mAsync(true)
	, pad2{0}
{
	Q_UNUSED(pad);
	Q_UNUSED(pad2);
}

MockService::~MockService()
{

}


void MockService::serviceActivateImp(bool on, ServiceActivatedCallback callBack)
{
	qDebug()<<"ACTIVATE("<<on<<" with async="<<mAsync<<") for "<<name();

	mWantActivated=on;
	mDirty=true;
	mCallBack=callBack;
	if(!mAsync) {
		mockSetActivated(nullptr!=callBack);
	}
}


/*
bool MockService::serviceActivatedImp() const
{
	return mActivated;
}
*/

// Simulate async activation
bool MockService::mockSetActivated(const bool runCallback)
{
	qDebug()<<"MOCK_SET_ACTIVATED(mWantActivated="<<mWantActivated<<", mActivated="<<mActivated<<", mDirty="<<mDirty<<", ) with runCallback="<<runCallback<<" for "<<name();
	if(mDirty) {
		mDirty=false;
		mActivated=mWantActivated;
		if(runCallback) {
			if(nullptr!=mCallBack) {
				mCallBack(mActivated, true);
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


void TestServiceManager::testHelpers()
{
	QSharedPointer<ServiceManager> man(OC_NEW ServiceManager());
	// Empty
	QCOMPARE(man->nameIsWellformed(""), false);
	QCOMPARE(man->nameIsWellformed(" "), false);
	QCOMPARE(man->nameIsWellformed("\n"), false);
	QCOMPARE(man->nameIsWellformed("\t"), false);
	QCOMPARE(man->nameIsWellformed("\n\t"), false);
	QCOMPARE(man->nameIsWellformed("\n\t "), false);
	QCOMPARE(man->nameIsWellformed("  "), false);

	// Space in start
	QCOMPARE(man->nameIsWellformed(" NAME"), false);
	QCOMPARE(man->nameIsWellformed("\nNAME"), false);
	QCOMPARE(man->nameIsWellformed("\tNAME"), false);
	QCOMPARE(man->nameIsWellformed("\n\tNAME"), false);
	QCOMPARE(man->nameIsWellformed("\n\t NAME"), false);
	QCOMPARE(man->nameIsWellformed("  NAME"), false);

	// Space in end
	QCOMPARE(man->nameIsWellformed("NAME "), false);
	QCOMPARE(man->nameIsWellformed("NAME\n"), false);
	QCOMPARE(man->nameIsWellformed("NAME\t"), false);
	QCOMPARE(man->nameIsWellformed("NAME\n\t"), false);
	QCOMPARE(man->nameIsWellformed("NAME\n\t "), false);
	QCOMPARE(man->nameIsWellformed("NAME  "), false);

	// Space in both ends
	QCOMPARE(man->nameIsWellformed(" NAME "), false);
	QCOMPARE(man->nameIsWellformed("\nNAME\n"), false);
	QCOMPARE(man->nameIsWellformed("\tNAME\t"), false);
	QCOMPARE(man->nameIsWellformed("\n\tNAME\n\t"), false);
	QCOMPARE(man->nameIsWellformed("\n\t NAME\n\t "), false);
	QCOMPARE(man->nameIsWellformed("  NAME  "), false);

	// No space in ends
	QCOMPARE(man->nameIsWellformed("NAME"), true);
	QCOMPARE(man->nameIsWellformed("NAME NAME"), true);
	QCOMPARE(man->nameIsWellformed("NAME\tNAME"), true);
	QCOMPARE(man->nameIsWellformed("NAME\nNAME"), true);
	QCOMPARE(man->nameIsWellformed("NAME\t\nNAME"), true);
	QCOMPARE(man->nameIsWellformed("NAME\t\n NAME"), true);
	QCOMPARE(man->nameIsWellformed("NAME  NAME"), true);

}


void TestServiceManager::testDirectRegistrationAndActivation()
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

	a->serviceChangeActivation(true, [](bool on, bool ok) {
		qDebug()<<"YAY CALLBACK! ON="<<on<<", OK="<<ok;
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

	b->serviceChangeActivation(true, [](bool on, bool ok) {
		qDebug()<<"YAY CALLBACK! ON="<<on<<", OK="<<ok;
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

	d->serviceChangeActivation(true, [](bool on, bool ok) {
		qDebug()<<"YAY CALLBACK! ON="<<on<<", OK="<<ok;
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


	a->serviceChangeActivation(true, [](bool on, bool ok) {
		qDebug()<<"YAY CALLBACK! ON="<<on<<", OK="<<ok;
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
	//QSet<QString> setA=man->metDependents("Service-A", false);
	QSet<QString> setA=man->metSet(man->dependents("Service-A"), false);

	man->changeActivation(setA, true, [=](bool ok) {
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
	a->serviceChangeActivation(false, [](bool on, bool ok) {
		qDebug()<<"Deactivated A: ON="<<on<<", OK="<<ok;
	});
	b->serviceChangeActivation(false, [](bool on, bool ok) {
		qDebug()<<"Deactivated B: ON="<<on<<", OK="<<ok;
	});
	c->serviceChangeActivation(false, [](bool on, bool ok) {
		qDebug()<<"Deactivated C: ON="<<on<<", OK="<<ok;
	});
	d->serviceChangeActivation(false, [](bool on, bool ok) {
		qDebug()<<"Deactivated D: ON="<<on<<", OK="<<ok;
	});

	QCOMPARE(a->mockSetActivated(true), false);
	QCOMPARE(b->mockSetActivated(true), false);
	QCOMPARE(c->mockSetActivated(true), false);
	QCOMPARE(d->mockSetActivated(true), false);

	QCOMPARE(a->serviceActiveActual(), false);
	QCOMPARE(b->serviceActiveActual(), false);
	QCOMPARE(c->serviceActiveActual(), false);
	QCOMPARE(d->serviceActiveActual(), false);


	QCOMPARE(a->serviceActiveWanted(), false);
	QCOMPARE(b->serviceActiveWanted(), false);
	QCOMPARE(c->serviceActiveWanted(), false);
	QCOMPARE(d->serviceActiveWanted(), false);

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

	man->changeActivation(man->all(), true, [](bool ok) {
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

	QCOMPARE(a->serviceActiveActual(), true);
	QCOMPARE(b->serviceActiveActual(), true);
	QCOMPARE(c->serviceActiveActual(), true);
	QCOMPARE(d->serviceActiveActual(), true);

	QCOMPARE(a->serviceActiveWanted(), true);
	QCOMPARE(b->serviceActiveWanted(), true);
	QCOMPARE(c->serviceActiveWanted(), true);
	QCOMPARE(d->serviceActiveWanted(), true);

	QCOMPARE(man->activatedWanted("Service-A"), true);
	QCOMPARE(man->activatedWanted("Service-B"), true);
	QCOMPARE(man->activatedWanted("Service-C"), true);
	QCOMPARE(man->activatedWanted("Service-D"), true);

	QCOMPARE(man->activatedActual("Service-A"), true);
	QCOMPARE(man->activatedActual("Service-B"), true);
	QCOMPARE(man->activatedActual("Service-C"), true);
	QCOMPARE(man->activatedActual("Service-D"), true);
}


void TestServiceManager::testServiceLevels()
{
	qDebug()<<"";
	qDebug()<<"";
	qDebug()<<"";
	qDebug()<<"=============================================================";
	qDebug()<<"=============================================================";
	qDebug()<<"=============================================================";
	qDebug()<<"";
	QSharedPointer<MockService> a(OC_NEW MockService("Service-A"));
	QSharedPointer<MockService> b(OC_NEW MockService("Service-B", QStringList{"Service-A"}));
	QSharedPointer<MockService> c(OC_NEW MockService("Service-C", QStringList{"Service-B", "Service-D"}));
	QSharedPointer<MockService> d(OC_NEW MockService("Service-D"));
	QSharedPointer<MockService> impostor_a(OC_NEW MockService("Service-A"));
	QSharedPointer<MockService> impostor_null;
	QSharedPointer<ServiceLevelManager> levelMan(OC_NEW ServiceLevelManager());
	QCOMPARE(levelMan->serviceManager().isNull(), true);
	levelMan->configure();
	QSharedPointer<ServiceManager> serviceMan=levelMan->serviceManager();
	QCOMPARE(serviceMan.isNull(), false);

	qDebug()<<"=============================================================";
	qDebug()<<"REGISTERING SERVICES";
	QCOMPARE(serviceMan->registered(a), false);
	QCOMPARE(levelMan->isServiceRegistered(a), false);
	QCOMPARE(levelMan->registerService(a), true);
	QCOMPARE(levelMan->isServiceRegistered(a), true);
	QCOMPARE(serviceMan->registered(a), true);

	QCOMPARE(serviceMan->registered(b), false);
	QCOMPARE(levelMan->isServiceRegistered(b), false);
	QCOMPARE(levelMan->registerService(b), true);
	QCOMPARE(levelMan->isServiceRegistered(b), true);
	QCOMPARE(serviceMan->registered(b), true);

	QCOMPARE(serviceMan->registered(c), false);
	QCOMPARE(levelMan->isServiceRegistered(c), false);
	QCOMPARE(levelMan->registerService(c), true);
	QCOMPARE(levelMan->isServiceRegistered(c), true);
	QCOMPARE(serviceMan->registered(c), true);

	QCOMPARE(serviceMan->registered(d), false);
	QCOMPARE(levelMan->isServiceRegistered(d), false);
	QCOMPARE(levelMan->registerService(d), true);
	QCOMPARE(levelMan->isServiceRegistered(d), true);
	QCOMPARE(serviceMan->registered(d), true);

	qDebug()<<"=============================================================";
	qDebug()<<"REGISTERING SERVICE LEVELS";
	QSharedPointer<ServiceLevel> sl1(OC_NEW ServiceLevel("Level-1", {c->name(), d->name()}));
	QCOMPARE(levelMan->isServiceLevelRegistered(sl1), false);
	QCOMPARE(levelMan->isServiceLevelRegistered(sl1->name()), false);
	levelMan->registerServiceLevel(sl1);
	QCOMPARE(levelMan->isServiceLevelRegistered(sl1), true);
	QCOMPARE(levelMan->isServiceLevelRegistered(sl1->name()), true);

	qDebug()<<"=============================================================";
	qDebug()<<"VERIFY NO ACTIVATED SERVICES";

	// Start with no levels enabled
	QCOMPARE(serviceMan->activatedWanted(a->name()), false);
	QCOMPARE(serviceMan->activatedWanted(b->name()), false);
	QCOMPARE(serviceMan->activatedWanted(c->name()), false);
	QCOMPARE(serviceMan->activatedWanted(d->name()), false);

	QCOMPARE(serviceMan->activatedActual(a->name()), false);
	QCOMPARE(serviceMan->activatedActual(b->name()), false);
	QCOMPARE(serviceMan->activatedActual(c->name()), false);
	QCOMPARE(serviceMan->activatedActual(d->name()), false);


	qDebug()<<"=============================================================";
	qDebug()<<"ENABLE SERVICE LEVEL";
	// Enable level 1
	levelMan->enableLevel(sl1->name(), true, [=](const bool ok) {
		qDebug()<<"=============================================================";
		qDebug()<<"LEVEL ENABLED";
		QCOMPARE(ok,true);
		// Check enabled services at completion
		QCOMPARE(serviceMan->activatedWanted(a->name()), true);
		QCOMPARE(serviceMan->activatedWanted(b->name()), true);
		QCOMPARE(serviceMan->activatedWanted(c->name()), true);
		QCOMPARE(serviceMan->activatedWanted(d->name()), true);
	});

	// Check enabled services before completion
	qDebug()<<"=============================================================";
	qDebug()<<"VERIFY NEW ACTIVATED SERVICES";
	QCOMPARE(serviceMan->activatedWanted(a->name()), true);
	QCOMPARE(serviceMan->activatedWanted(b->name()), false);
	QCOMPARE(serviceMan->activatedWanted(c->name()), false);
	QCOMPARE(serviceMan->activatedWanted(d->name()), true);

	// Step closer to completion by running async code
	qDebug()<<"=============================================================";
	qDebug()<<"STEP MOCK ACTIVATION";
	a->mockSetActivated(true);
	d->mockSetActivated(true);
	b->mockSetActivated(true);
	c->mockSetActivated(true);

	// Check enabled services after completion
	qDebug()<<"=============================================================";
	qDebug()<<"VERIFY ALL ACTIVATED SERVICES";
	QCOMPARE(serviceMan->activatedWanted(a->name()), true);
	QCOMPARE(serviceMan->activatedWanted(b->name()), true);
	QCOMPARE(serviceMan->activatedWanted(c->name()), true);
	QCOMPARE(serviceMan->activatedWanted(d->name()), true);

	qDebug()<<"=============================================================";
	qDebug()<<"Haxta lavixta baxby";
}


OC_TEST_MAIN(test, TestServiceManager)

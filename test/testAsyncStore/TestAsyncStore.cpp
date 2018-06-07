#include "TestAsyncStore.hpp"

#include "node/AsyncStore.hpp"

template<typename T>
class BackendMock: public AsyncBackend<T>
{
public:
	QString filename;
	bool exists;
	T diskData;

	bool doFailClear;
	bool doFailSave;
	bool doFailLoad;

public:

	explicit BackendMock(QString f="", bool exists=false, T savedData=T(), bool doFailClear=false, bool doFailSave=false, bool doFailLoad=false)
		: filename(f)
		, exists(exists)
		, diskData(savedData)
		, doFailClear(doFailClear)
		, doFailSave(doFailSave)
		, doFailLoad(doFailLoad)
	{
		qDebug().nospace().noquote()<<"BackendMock::BackendMock(filename='"<<filename<<"', exists="<<exists<<", savedData="<<savedData<<", doFailClear="<<doFailClear<<", doFailSave="<<doFailSave<<", doFailLoad="<<doFailLoad<<") @ "<<utility::currentThreadID();
	}
	// AsyncBackend interface
public:
	void setFilenameBackend(QString f) Q_DECL_OVERRIDE{
		this->filename=f;
	}


	QString filenameBackend()  Q_DECL_OVERRIDE{
		return filename;
	}

	bool existsBackend()  Q_DECL_OVERRIDE{
		return exists;
	}

	bool clearBackend() Q_DECL_OVERRIDE{
		bool ret=true;
		if(doFailClear)
		{
			ret=false;
		} else{
			exists = false;
			diskData = T();
		}
		qDebug().nospace().noquote()<<"clearBackend() with doFailClear="<<doFailClear<<" returned "<<ret<< " @ "<<utility::currentThreadID();
		return ret;
	}

	bool saveBackend(T data)  Q_DECL_OVERRIDE{
		bool ret=true;
		if(doFailSave)
		{
			ret=false;
		} else{
			this->diskData=data;
			this->exists=true;
		}
		qDebug().nospace().noquote()<<"saveBackend() with doFailSave="<<doFailSave<<" returned "<<ret<< " @ "<<utility::currentThreadID();
		return ret;
	}

	T loadBackend(bool &ok)  Q_DECL_OVERRIDE{
		if(doFailLoad)
		{
			ok=false;
			T data=T();
			qDebug().nospace().noquote()<<"loadBackend("<<ok<<") with doFailLoad="<<doFailLoad<<" returned "<<data<< " @ "<<utility::currentThreadID();
			return data;
		} else{
			if(exists)
			{
				ok=true;
				qDebug().nospace().noquote()<<"loadBackend("<<ok<<") with doFailLoad="<<doFailLoad<<" returned "<<diskData<< " @ "<<utility::currentThreadID();
				return diskData;
			} else{
				ok=false;
				T data=T();
				qDebug().nospace().noquote()<<"loadBackend("<<ok<<") with doFailLoad="<<doFailLoad<<" returned "<<data<< " @ "<<utility::currentThreadID();
				return data;
			}
		}
	}
};


template<typename T>
class FrontendMock: public AsyncFrontend<T>
{
public:
	bool exists;
	T memData;
	T generateData;

	bool doFailClear;
	bool doFailSet;
	bool doFailGet;
	bool doFailGenerate;
public:

	explicit FrontendMock(bool exists=false, T openData=T(), T generateData=T(), bool doFailClear=false, bool doFailSet=false, bool doFailGet=false, bool doFailGenerate=false)
		: exists(exists)
		, memData(openData)
		, generateData(generateData)
		, doFailClear(doFailClear)
		, doFailSet(doFailSet)
		, doFailGet(doFailGet)
		, doFailGenerate(doFailGenerate)
	{
		qDebug().nospace().noquote()<<"FrontendMock::FrontendMock(exists="<<exists<<", openData="<<openData<<", generateData="<<generateData<<", doFailClear="<<doFailClear<<", doFailSet="<<doFailSet<<", doFailGet="<<doFailGet<<", doFailGenerate="<<doFailGenerate<<") @ "<<utility::currentThreadID();
	}
	// AsyncFrontend interface
public:

	bool clearFrontend() Q_DECL_OVERRIDE{
		bool ret=true;
		if(doFailClear)
		{
			ret=false;
		} else{
			exists=false;
			memData=T();
		}
		qDebug().nospace().noquote()<<"clearFrontend() with doFailClear="<<doFailClear<<" returned "<<ret<< " @ "<<utility::currentThreadID();
		return ret;
	}

	bool setFrontend(T data) Q_DECL_OVERRIDE{
		bool ret=true;
		if(doFailSet)
		{
			ret=false;
		} else{
			memData=data;
			exists=true;
		}
		qDebug().nospace().noquote()<<"setFrontend("<<memData<<") with doFailSet="<<doFailSet<<" returned "<<ret<< " @ "<<utility::currentThreadID();
		return ret;
	}

	T getFrontend(bool &ok) Q_DECL_OVERRIDE{
		if(doFailGet)
		{
			ok=false;
			T data=T();
			qDebug().nospace().noquote()<<"getFrontend("<<ok<<") with doFailGet="<<doFailGet<<" returned "<<data<< " @ "<<utility::currentThreadID();
			return data;
		} else{
			if(exists)
			{
				ok=true;
				qDebug().nospace().noquote()<<"getFrontend("<<ok<<") with doFailGet="<<doFailGet<<" returned "<<memData<< " @ "<<utility::currentThreadID();
				return memData;
			} else{
				ok=false;
				T data=T();
				qDebug().nospace().noquote()<<"getFrontend("<<ok<<") with doFailGet="<<doFailGet<<" returned "<<data<< " @ "<<utility::currentThreadID();
				return data;
			}
		}
	}

	bool generateFrontend() Q_DECL_OVERRIDE{
		bool ret=true;
		if(doFailGenerate)
		{
			ret=false;
		} else{
			memData=generateData;
			QThread::msleep(1000);
			exists=true;
		}
		qDebug().nospace().noquote()<<"generateFrontend() with doFailGenerate="<<doFailGenerate<<" returned "<<ret<< " @ "<<utility::currentThreadID();
		return ret;
	}

};


template<typename T>
class AsyncStoreTester
{
public:
	BackendMock<T> backend;
	FrontendMock<T> frontend;
	AsyncStore<T> store;

public:
	explicit AsyncStoreTester<T>(QString backendFilename
								 , bool backendExists
								 , T backendData
								 , bool frontendExists
								 , T frontendData
								 , T frontendGeneratorData
								 , bool backendDoFailClear=false
										 , bool backendDoFailSave=false
												 , bool backendDoFailLoad=false
														 , bool frontendDoFailClear=false
																 , bool frontendDoFailSet=false
																		 , bool frontendDoFailGet=false
																				 , bool frontendDoFailGenerate=false);


	virtual ~AsyncStoreTester<T>();
};


template<typename T>
AsyncStoreTester<T>::AsyncStoreTester(QString backendFilename
									  , bool backendExists
									  , T backendData
									  , bool frontendExists
									  , T frontendData
									  , T frontendGeneratorData
									  , bool backendDoFailClear
									  , bool backendDoFailSave
									  , bool backendDoFailLoad
									  , bool frontendDoFailClear
									  , bool frontendDoFailSet
									  , bool frontendDoFailGet
									  , bool frontendDoFailGenerate )
	: backend(backendFilename, backendExists, backendData, backendDoFailClear, backendDoFailSave, backendDoFailLoad)
	, frontend(frontendExists, frontendData, frontendGeneratorData, frontendDoFailClear, frontendDoFailSet, frontendDoFailGet, frontendDoFailGenerate)
	, store(backend, frontend)
{
	qDebug().nospace().noquote()<<"AsyncStoreTester::AsyncStoreTester()";
}

template<typename T>
AsyncStoreTester<T>::~AsyncStoreTester<T>()
{
	qDebug().nospace().noquote()<<"AsyncStoreTester::~AsyncStoreTester()";
}

void TestAsyncStore::testFileExists()
{
	typedef int TestType;
	{
		AsyncStoreTester<TestType> ast("dummy.json", true, 1234, false, 0, 1337);
		qDebug().nospace().noquote()<<"Sync "<<utility::currentThreadID();
		//QVERIFY(ast.);
		ast.store.synchronize().onFinished([](ASEvent<TestType> &a) {
			qDebug().nospace().noquote()<<"AsyncStoreTest::synchronize().onFinished() "<<utility::currentThreadID();
		});

		qDebug().nospace().noquote()<<"Done with disk=" << ast.backend.diskData<< "(" << ast.backend.exists<< "), mem="<< ast.frontend.memData<< " @ "<<utility::currentThreadID();
	}
	qDebug().nospace().noquote()<<"Dead @ "<<utility::currentThreadID();
}

void TestAsyncStore::testGenerate()
{
	typedef int TestType;
	{
		AsyncStoreTester<TestType> ast("dummy.json", false, 1234, false, 0, 1337);
		qDebug().nospace().noquote()<<"Sync "<<utility::currentThreadID();
		ast.store.synchronize().onFinished([](ASEvent<TestType> &a) {
			qDebug().nospace().noquote()<<"AsyncStoreTest::synchronize().onFinished() "<<utility::currentThreadID();
		});
		qDebug().nospace().noquote()<<"Done with disk=" << ast.backend.diskData<< "(" << ast.backend.exists<< "), mem="<< ast.frontend.memData<< " @ "<<utility::currentThreadID();
	}
	qDebug().nospace().noquote()<<"Dead @ "<<utility::currentThreadID();
}

void TestAsyncStore::testSaveChange()
{
	typedef int TestType;
	{
		AsyncStoreTester<TestType> ast("dummy.json", true, 1234, false, 0, 1337);
		qDebug().nospace().noquote()<<"Sync "<<utility::currentThreadID();
		ast.store.synchronize().onFinished([](ASEvent<TestType> &a) {
			qDebug().nospace().noquote()<<"AsyncStoreTest::synchronize().onFinished() "<<utility::currentThreadID();
		});

		ast.store.set(666).onFinished([](ASEvent<TestType> &a) {
			qDebug().nospace().noquote()<<"AsyncStoreTest::set().onFinished() "<<utility::currentThreadID();
		});

		qDebug().nospace().noquote()<<"Done with disk=" << ast.backend.diskData<< "(" << ast.backend.exists<< "), mem="<< ast.frontend.memData<< " @ "<<utility::currentThreadID();
	}
	qDebug().nospace().noquote()<<"Dead @ "<<utility::currentThreadID();
}



void TestAsyncStore::testFails()
{

}


OC_TEST_MAIN(test, TestAsyncStore)


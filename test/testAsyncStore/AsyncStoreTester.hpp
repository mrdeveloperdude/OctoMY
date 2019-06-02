#ifndef ASYNCSTORETESTER_HPP
#define ASYNCSTORETESTER_HPP

#include "BackendMock.hpp"

#include "FrontendMock.hpp"


template<typename T>
class AsyncStoreTester
{
public:
	QSharedPointer<BackendMock<T> > backend;
	QSharedPointer<FrontendMock<T> >frontend;
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


	void configure();
	void activate(bool on);
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
	: backend( new BackendMock<T>(backendFilename, backendExists, backendData, backendDoFailClear, backendDoFailSave, backendDoFailLoad))
	, frontend( new FrontendMock<T>(frontendExists, frontendData, frontendGeneratorData, frontendDoFailClear, frontendDoFailSet, frontendDoFailGet, frontendDoFailGenerate))
{
	qDebug().nospace().noquote()<<"AsyncStoreTester::AsyncStoreTester()";
}

template<typename T>
AsyncStoreTester<T>::~AsyncStoreTester<T>()
{
	qDebug().nospace().noquote()<<"AsyncStoreTester::~AsyncStoreTester()";
}



template<typename T>
void AsyncStoreTester<T>::configure()
{
	qDebug().nospace().noquote()<<"AsyncStoreTester::configure()";
	store.configure(backend, frontend);
}


template<typename T>
void AsyncStoreTester<T>::activate(bool on)
{
	qDebug().nospace().noquote()<<"AsyncStoreTester::activate("<<on<<")";
	store.activate(on);
}



#endif
// ASYNCSTORETESTER_HPP

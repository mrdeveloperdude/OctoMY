#ifndef ASYNCSTORETESTER_HPP
#define ASYNCSTORETESTER_HPP

#include "BackendMock.hpp"

#include "FrontendMock.hpp"


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



#endif // ASYNCSTORETESTER_HPP

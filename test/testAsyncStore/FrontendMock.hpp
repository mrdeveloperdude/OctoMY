#ifndef FRONTENDMOCK_HPP
#define FRONTENDMOCK_HPP


#include "node/AsyncStore.hpp"

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


	virtual ~FrontendMock()
	{

	}

	// AsyncFrontend interface
public:

	bool clearFrontend() Q_DECL_OVERRIDE{
		bool ret=true;
		if(doFailClear)
		{
			ret=false;
		} else
		{
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
		} else
		{
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
		} else
		{
			if(exists) {
				ok=true;
				qDebug().nospace().noquote()<<"getFrontend("<<ok<<") with doFailGet="<<doFailGet<<" returned "<<memData<< " @ "<<utility::currentThreadID();
				return memData;
			} else {
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
		} else
		{
			memData=generateData;
			QThread::msleep(1000);
			exists=true;
		}
		qDebug().nospace().noquote()<<"generateFrontend() with doFailGenerate="<<doFailGenerate<<" returned "<<ret<< " @ "<<utility::currentThreadID();
		return ret;
	}

};



#endif // FRONTENDMOCK_HPP

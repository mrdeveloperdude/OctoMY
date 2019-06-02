#ifndef FRONTENDMOCK_HPP
#define FRONTENDMOCK_HPP

#include "Common_test.hpp"

#include "store/AsyncStore.hpp"


#include <QDebug>

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

	static int sid;
	int id=sid++;
	bool doLog;
public:

	explicit FrontendMock(bool exists=false, T openData=T(), T generateData=T(), bool doFailClear=false, bool doFailSet=false, bool doFailGet=false, bool doFailGenerate=false, bool doLog=false)
		: exists(exists)
		, memData(openData)
		, generateData(generateData)
		, doFailClear(doFailClear)
		, doFailSet(doFailSet)
		, doFailGet(doFailGet)
		, doFailGenerate(doFailGenerate)
		, doLog(doLog)
	{
		if(doLog) {
			qDebug().nospace().noquote()<<"FrontendMock::FrontendMock(#"<<id<<"/"<<sid<<", exists="<<exists<<", openData="<<openData<<", generateData="<<generateData<<", doFailClear="<<doFailClear<<", doFailSet="<<doFailSet<<", doFailGet="<<doFailGet<<", doFailGenerate="<<doFailGenerate<<") @ "<<utility::concurrent::currentThreadID();
		}
	}


	virtual ~FrontendMock()
	{
		if(doLog) {
			qDebug().nospace().noquote()<<"FrontendMock::~FrontendMock(#"<<id<<"/"<<sid<<") @ "<<utility::concurrent::currentThreadID();
		}

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
		if(doLog)
		{
			qDebug().nospace().noquote()<<"FrontendMock::clearFrontend(#"<<id<<"/"<<sid<<") with doFailClear="<<doFailClear<<" returned "<<ret<< " @ "<<utility::concurrent::currentThreadID();
		}
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
		if(doLog)
		{
			//qDebug()<<"setFrontend("<<memData;//<<") with doFailSet="<<doFailSet<<" returned "<<ret<< " @ "<<utility::concurrent::currentThreadID();
			qDebug().nospace().noquote()<<"FrontendMock::setFrontend(#"<<id<<"/"<<sid<<", data="<<memData<<") with doFailSet="<<doFailSet<<" returned "<<ret<< " @ "<<utility::concurrent::currentThreadID();
		}
		return ret;
	}

	T getFrontend(bool &ok) Q_DECL_OVERRIDE{
		if(doFailGet)
		{
			ok=false;
			T data=T();
			if(doLog) {
				qDebug().nospace().noquote()<<"FrontendMock::getFrontend(#"<<id<<"/"<<sid<<", ok="<<ok<<") with doFailGet="<<doFailGet<<" returned "<<data<< " @ "<<utility::concurrent::currentThreadID();
			}
			return data;
		} else
		{
			if(exists) {
				ok=true;
				if(doLog) {
					qDebug().nospace().noquote()<<"FrontendMock::getFrontend(#"<<id<<"/"<<sid<<", ok="<<ok<<") with doFailGet="<<doFailGet<<" returned "<<memData<< " @ "<<utility::concurrent::currentThreadID();
				}
				return memData;
			} else {
				ok=false;
				T data=T();
				if(doLog) {
					qDebug().nospace().noquote()<<"FrontendMock::getFrontend(#"<<id<<"/"<<sid<<", ok="<<ok<<") with doFailGet="<<doFailGet<<" returned "<<data<< " @ "<<utility::concurrent::currentThreadID();
				}
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
			QThread::msleep(10+qrand()%100);
			exists=true;
		}
		if(doLog)
		{
			qDebug().nospace().noquote()<<"FrontendMock::generateFrontend(#"<<id<<"/"<<sid<<", ) with doFailGenerate="<<doFailGenerate<<" returned "<<ret<< " @ "<<utility::concurrent::currentThreadID();
		}
		return ret;
	}

};

template<typename T>
int FrontendMock<T>::sid=0;

#endif // FRONTENDMOCK_HPP

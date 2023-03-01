#ifndef BACKENDMOCK_HPP
#define BACKENDMOCK_HPP

#include "test/Common.hpp"

#include "store/AsyncStore.hpp"

#include <QDebug>

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

	static int sid;
	int id=sid++;
	bool doLog;

public:
	explicit BackendMock(QString f="", bool exists=false, T savedData=T(), bool doFailClear=false, bool doFailSave=false, bool doFailLoad=false, bool doLog=false);
	virtual ~BackendMock();

	// AsyncBackend interface
public:
	void setFilenameBackend(QString f) Q_DECL_OVERRIDE;
	QString filenameBackend()  Q_DECL_OVERRIDE;
	bool existsBackend()  Q_DECL_OVERRIDE;
	bool clearBackend() Q_DECL_OVERRIDE;
	bool saveBackend(T data)  Q_DECL_OVERRIDE;
	T loadBackend(bool &ok)  Q_DECL_OVERRIDE;
};


template<typename T>
BackendMock<T>::BackendMock(QString f, bool exists, T savedData, bool doFailClear, bool doFailSave, bool doFailLoad, bool doLog)
	: filename(f)
	, exists(exists)
	, diskData(savedData)
	, doFailClear(doFailClear)
	, doFailSave(doFailSave)
	, doFailLoad(doFailLoad)
	, doLog(doLog)
{
	if(doLog) {
		qDebug().nospace().noquote()<<"BackendMock::BackendMock(#"<<id<<"/"<<sid<<", filename='"<<filename<<"', exists="<<exists<<", savedData="<<savedData<<", doFailClear="<<doFailClear<<", doFailSave="<<doFailSave<<", doFailLoad="<<doFailLoad<<") @ "<<utility::concurrent::currentThreadID();
	}
}


template<typename T>
BackendMock<T>::~BackendMock()
{
	if(doLog) {
		qDebug().nospace().noquote()<<"BackendMock::~BackendMock(#"<<id<<"/"<<sid<<") @ "<<utility::concurrent::currentThreadID();
	}
}


template<typename T>
void BackendMock<T>::setFilenameBackend(QString f){
	this->filename=f;
}


template<typename T>
QString BackendMock<T>::filenameBackend(){
	return filename;
}


template<typename T>
bool BackendMock<T>::existsBackend(){
	return exists;
}


template<typename T>
bool BackendMock<T>::clearBackend(){
	bool ret=true;
	if(doFailClear)
	{
		ret=false;
	} else
	{
		exists = false;
		diskData = T();
	}
	if(doLog)
	{
		qDebug().nospace().noquote()<<"BackendMock::clearBackend(#"<<id<<"/"<<sid<<") with doFailClear="<<doFailClear<<" returned "<<ret<< " @ "<<utility::concurrent::currentThreadID();
	}
	return ret;
}


template<typename T>
bool BackendMock<T>::saveBackend(T data){
	bool ret=true;
	if(doFailSave)
	{
		ret=false;
	} else
	{
		this->diskData=data;
		this->exists=true;
	}
	if(doLog)
	{
		qDebug().nospace().noquote()<<"BackendMock::saveBackend(#"<<id<<"/"<<sid<<") with doFailSave="<<doFailSave<<" returned "<<ret<< " @ "<<utility::concurrent::currentThreadID();
	}
	return ret;
}


template<typename T>
T BackendMock<T>::loadBackend(bool &ok){
	if(doFailLoad)
	{
		ok=false;
		T data=T();
		if(doLog) {
			qDebug().nospace().noquote()<<"BackendMock::loadBackend(#"<<id<<"/"<<sid<<", ok="<<ok<<") with doFailLoad="<<doFailLoad<<" returned "<<data<< " @ "<<utility::concurrent::currentThreadID();
		}
		return data;
	} else
	{
		if(exists) {
			ok=true;
			if(doLog) {
				qDebug().nospace().noquote()<<"BackendMock::loadBackend(#"<<id<<"/"<<sid<<", ok="<<ok<<") with doFailLoad="<<doFailLoad<<" returned "<<diskData<< " @ "<<utility::concurrent::currentThreadID();
			}
			return diskData;
		} else {
			ok=false;
			T data=T();
			if(doLog) {
				qDebug().nospace().noquote()<<"BackendMock::loadBackend(#"<<id<<"/"<<sid<<", ok="<<ok<<") with doFailLoad="<<doFailLoad<<" returned "<<data<< " @ "<<utility::concurrent::currentThreadID();
			}
			return data;
		}
	}
}

template<typename T>
int BackendMock<T>::sid=0;


#endif
// BACKENDMOCK_HPP

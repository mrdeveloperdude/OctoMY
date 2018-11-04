#ifndef BACKENDMOCK_HPP
#define BACKENDMOCK_HPP


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
		} else
		{
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
		} else
		{
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
		} else
		{
			if(exists) {
				ok=true;
				qDebug().nospace().noquote()<<"loadBackend("<<ok<<") with doFailLoad="<<doFailLoad<<" returned "<<diskData<< " @ "<<utility::currentThreadID();
				return diskData;
			} else {
				ok=false;
				T data=T();
				qDebug().nospace().noquote()<<"loadBackend("<<ok<<") with doFailLoad="<<doFailLoad<<" returned "<<data<< " @ "<<utility::currentThreadID();
				return data;
			}
		}
	}
};



#endif // BACKENDMOCK_HPP

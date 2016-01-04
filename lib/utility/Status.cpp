#include "Status.hpp"

#include "basic/Standard.hpp"

namespace utility{
	Status::Status(STATUS s,const QString m,int code):
		mStatus(s)
	  , mMessage(m)
	  , mCode(code)
	{
		WWMETHODGATE();
	}

	Status::Status(const QString m,STATUS s,int code):
		mStatus(s)
	  , mMessage(m)
	  , mCode(code)
	{
		WWMETHODGATE();
	}

	Status::Status(bool ok,const QString m,int code):
		mStatus(ok?OK:ERROR)
	  , mMessage(m)
	  , mCode(code)
	{
		WWMETHODGATE();

	}

	Status::Status(const Status &s):
		mStatus(s.mStatus)
	  , mMessage(s.mMessage)
	  , mCode(s.mCode)
	{
		WWMETHODGATE();
	}

	Status::~Status(){
		WWMETHODGATE();
	}



	void Status::setStatus(STATUS s,const QString m,int c){
		WWMETHODGATE();
		mMessage=m;
		mStatus=s;
		mCode=c;
	}

	void Status::setStatus(const QString m,STATUS s, int c){
		WWMETHODGATE();
		mMessage=m;
		mStatus=s;
		mCode=c;
	}

	void Status::setStatus(bool ok,const QString m,int c){
		WWMETHODGATE();
		mMessage=m;
		mStatus=ok?OK:ERROR;
		mCode=c;
	}

	void Status::setStatus(const Status &s){
		WWMETHODGATE();
		mMessage=s.mMessage;
		mStatus=s.mStatus;
		mCode=s.mStatus;
	}

	void Status::setOK(){
		WWMETHODGATE();
		setStatus(OK,"",0);
	}

	Status::STATUS Status::getStatus()const{
		WWMETHODGATE();
		return mStatus;
	}
	QString Status::getStatusMessage()const{
		WWMETHODGATE();
		return mMessage;
	}
	int Status::getStatusCode()const{
		return mCode;
	}

	bool Status::isOK()const{
		WWMETHODGATE();
		return OK==mStatus;
	}

	QString Status::toString(){
		WWMETHODGATE();
		if(isOK()){
			return "STATUS{OK}";
		}
		return "STATUS{ERROR MSG=" +getStatusMessage()+", CODE=" +QString::number(getStatusCode())+"}";
	}

}

QDebug operator<<(QDebug d, utility::Status &s){
	WWFUNCTIONGATE();
	d.nospace() << s.toString();
	return d.space();
}

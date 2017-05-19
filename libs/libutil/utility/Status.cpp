#include "Status.hpp"

#include "utility/Standard.hpp"

namespace utility{
	Status::Status(STATUS s,const QString m,int code):
		mStatus(s)
	  , mMessage(m)
	  , mCode(code)
	{
		OC_METHODGATE();
	}

	Status::Status(const QString m,STATUS s,int code):
		mStatus(s)
	  , mMessage(m)
	  , mCode(code)
	{
		OC_METHODGATE();
	}

	Status::Status(bool ok,const QString m,int code):
		mStatus(ok?OK:ERROR)
	  , mMessage(m)
	  , mCode(code)
	{
		OC_METHODGATE();

	}

	Status::Status(const Status &s):
		mStatus(s.mStatus)
	  , mMessage(s.mMessage)
	  , mCode(s.mCode)
	{
		OC_METHODGATE();
	}

	Status::~Status(){
		OC_METHODGATE();
	}



	void Status::setStatus(STATUS s,const QString m,int c){
		OC_METHODGATE();
		mMessage=m;
		mStatus=s;
		mCode=c;
	}

	void Status::setStatus(const QString m,STATUS s, int c){
		OC_METHODGATE();
		mMessage=m;
		mStatus=s;
		mCode=c;
	}

	void Status::setStatus(bool ok,const QString m,int c){
		OC_METHODGATE();
		mMessage=m;
		mStatus=ok?OK:ERROR;
		mCode=c;
	}

	void Status::setStatus(const Status &s){
		OC_METHODGATE();
		mMessage=s.mMessage;
		mStatus=s.mStatus;
		mCode=s.mStatus;
	}

	void Status::setOK(){
		OC_METHODGATE();
		setStatus(OK,"",0);
	}

	Status::STATUS Status::getStatus()const{
		OC_METHODGATE();
		return mStatus;
	}
	QString Status::getStatusMessage()const{
		OC_METHODGATE();
		return mMessage;
	}
	int Status::getStatusCode()const{
		return mCode;
	}

	bool Status::isOK()const{
		OC_METHODGATE();
		return OK==mStatus;
	}

	QString Status::toString(){
		OC_METHODGATE();
		if(isOK()){
			return "STATUS{OK}";
		}
		return "STATUS{ERROR MSG=" +getStatusMessage()+", CODE=" +QString::number(getStatusCode())+"}";
	}

}

QDebug operator<<(QDebug d, utility::Status &s){
	OC_FUNCTIONGATE();
	d.nospace() << s.toString();
	return d.space();
}

#ifndef STATUS_HPP
#define STATUS_HPP

#include <QString>

namespace utility
{
class Status
{
public:
	enum STATUS {
		OK,ERROR
	};
private:
	STATUS mStatus;
	QString mMessage;
	int mCode;
public:
	explicit Status(STATUS s=OK,const QString m="",int code=0);
	explicit Status(const QString m,STATUS s=OK,int code=0);
	explicit Status(bool ok,QString m="",int code=0);
	Status(const Status &s);
	virtual ~Status();

public:
	STATUS getStatus() const;
	QString getStatusMessage() const;
	int getStatusCode() const;
	bool isOK() const;

	void setStatus(STATUS s,QString m="",int code=0);
	void setStatus(QString m,STATUS s=OK,int code=0);
	void setStatus(bool ok,QString m="",int code=0);
	void setStatus(const Status &s);
	void setOK();


	QString toString();

};
}


QDebug operator<<(QDebug d, utility::Status &s);

#endif // STATUS_HPP

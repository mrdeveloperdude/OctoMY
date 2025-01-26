#ifndef UNIQUEID_HPP
#define UNIQUEID_HPP

#include <QString>

QString generateUniqueID();

class UniqueID{
private:
	QString mUniqueID;
	int mID;

public:
	UniqueID();

public:
	inline QString uniqueID() const {
		return mUniqueID;
	}
	inline int id() const {
		return mID;
	}
	
	friend QDataStream& operator<<(QDataStream& out, const UniqueID& obj);
	friend QDataStream& operator>>(QDataStream& in, UniqueID& obj);
	
};


QDataStream& operator<<(QDataStream& out, const UniqueID& obj);
QDataStream& operator>>(QDataStream& in, UniqueID& obj);



#endif // UNIQUEID_HPP

#ifndef STORE_HPP
#define STORE_HPP

#include <QVariant>
#include <QChar>

#include <functional>


namespace  store{


enum KeyTokenType: quint8{
	NONE // Unspecified. Indicates uninitialized token
	, ERROR // Error occured, name will contain error text
	, DIRECTORY // Coresponging to a directory on disk
	, ITEM // Corresponding to an item in a list
	, PROPERTY // Corresponding to a property in a map
};

QString KeyTokenTypeToString(KeyTokenType type);


struct KeyToken{
	KeyTokenType type;
	QString name;
	int nameIndex; // Where in the string was this token found?
	int itemIndex; // Only valid for type=item


	KeyToken(KeyTokenType type,
			 QString name,
			 int nameIndex)
		: type(type)
		, name(name)
		, nameIndex(nameIndex)
		, itemIndex(0) {

	}

	bool valid() const;

	QString toString() const;
	operator QString() const;
	operator QString();
};


struct KeyParser{
	KeyToken last;
	int index;
	QString str;
	KeyParser(QString str);
	KeyToken parsePart();
	QList<KeyToken> parse();
};

class Status{
private:
	bool mOK;
	QString mError;

public:
	Status():mOK(true){}
	Status(QString message):mOK(false), mError(message){}

public:
	QString toString() const;
	operator QString() const;

};

class Key{
private:
	QString mStr;
	QList<KeyToken> mPath;

public:
	Key(QString str);

public:
	void set(QString str);

private:
	void parse();

public:
	QVariant lookUp(QVariant &root) const;
	Status put (QVariant &root, QVariant data);

public:
	QString toString() const;
	operator QString() const;
	operator QString();

};

class Context
{
private:
	QVariant mData;

public:
	Context();

public:
	void get(Key key, std::function<void (Status status, QVariant data)> cb);
	void put(Key key, std::function<void (Status status)> cb, QVariant data);

public:
	QString toString() const;
	operator QString() const;
	operator QString();

};

QString vtos(QVariant v);


}


//const QDebug &operator<<(QDebug &d, const store::Status &status);

#endif
// STORE_HPP

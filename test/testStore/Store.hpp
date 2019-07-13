#ifndef STORE_HPP
#define STORE_HPP

#include <QVariant>
#include <QChar>

#include <functional>


namespace  store{



enum TokenType: quint8{
	INVALID, LETTER, NUMBER, OPEN_BRACE, CLOSE_BRACE, COMMA, POINT, WHITESPACE
};

struct Token{
	int index;
	QChar c;
	TokenType type;
	Token(int index, QChar c)
		: index(index)
		, c(c)
		, type(INVALID)
	{
		if(c.isLetter()){
			type=LETTER;
		} else if(c.isNumber()){
			type=NUMBER;
		} else if('['==c){
			type=OPEN_BRACE;
		} else if(']'==c){
			type=CLOSE_BRACE;
		} else if(','==c){
			type=COMMA;
		} else if('.'==c){
			type=POINT;
		} else if(c.isSpace()){
			type=WHITESPACE;
		}
	}

};

enum KeyTokenType: quint8{
	DIRECTORY // Coresponging to a directory on disk
	, ITEM // Corresponding to an item in a list
	, PROPERTY // Corresponding to a property in a map
};

struct KeyToken{
	KeyTokenType type;
	QString name;
	int index;// Only valid for type=item
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
	QVector<KeyToken> mPath;

public:
	Key(QString str);

public:
	void set(QString str);

private:
	Token parseToken(int i);
	void parse();

public:
	QVariant lookUp(QVariantMap root) const;
	Status put (QVariantMap &root, QVariant data);

public:
	QString toString() const;
	operator QString() const;

};

class Context
{
private:
	QVariantMap mData;

public:
	Context();

public:
	void get(Key key, std::function<void (Status status, QVariant data)> cb);
	void put(Key key, std::function<void (Status status)> cb, QVariant data);

public:
	QString toString() const;
	operator QString() const;

};

QString vtos(QVariant v);


}


//const QDebug &operator<<(QDebug &d, const store::Status &status);

#endif
// STORE_HPP

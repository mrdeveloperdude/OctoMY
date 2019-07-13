#include "Store.hpp"

#include <QDebug>
#include <QStack>



namespace  store{

QString Status::toString() const
{
	return mOK?"OK":("ERROR: "+mError);
}


Status::operator QString() const
{
	return toString();
}


Key::Key(QString str)
	: mStr(str.trimmed())
{
	parse();
}


void Key::set(QString str)
{
	mStr=str;
	parse();
}

Token Key::parseToken(int i)
{
	return Token(i, mStr[i]);
}

void Key::parse(){
	Token lastToken(-1, 0);
	for (int i=0, sz=mStr.size(); i<sz; ++i){
		Token token=parseToken(i);
		switch(lastToken.type){
			case(INVALID):{
			}break;
		}
	}
	mPath=mStr.split(".", QString::SkipEmptyParts);
}




class Lookup{
private:
	Key &mKey;
	QVariant &mRoot, &mIndex;

public:
	Lookup(Key &key, QVariant &root)
		: mKey(key)
		, mRoot(root)
		, mIndex(mRoot)
	{

	}

private:

public:
	bool lookUp(QString part){

		switch(mIndex.type()){
			case(QVariant::Map):{
				auto map=qvariant_cast<QVariantMap &>(mIndex);
				mIndex=map[part];
			}break;
			case(QVariant::List):{
				auto list=qvariant_cast<QVariantList &>(mIndex);
				const auto sz=list.size();
			}break;
			default:
				ret+=v.toString();
		}


		if(QVariant::List==root.type()){
			bool ok=false;
			int num=part.toInt(&ok);
			if(!ok){
				return false;
			}

			return lookUp(root.toList()[num]);
		}

	}


};


QVariant Key::lookUp(QVariantMap root) const
{
	QVariantMap &pointer=root;
	auto w=mPath.mid(0,mPath.size()-1);
	auto l=mPath.last();
	for(QString part:w){
		qDebug()<<"Looking up part "<<part;
		if(!pointer.contains(part)){
			pointer[part]=QVariantMap();
		}
		QVariant &v=pointer[part];
		if(QVariant::Map!=v.type()){
			return QVariant();
		}
		pointer=v.toMap();
	}
	return pointer[l];
}


Status Key::put(QVariantMap &root, QVariant data)
{
	QVariantMap &pointer=root;
	auto w=mPath.mid(0,mPath.size()-1);
	auto l=mPath.last();
	qDebug()<<"W="<<w;
	qDebug()<<"L="<<l;

	//QStack<QString> stack;
	for(QString part:w){
		qDebug()<<"Traversing part "<<part;
		QVariant v;
		if(pointer.contains(part)){
			v=pointer[part];
		}
		else{
			v=QVariantMap();
		}
		qDebug()<<"PART: "<<vtos(v);
		if(QVariant::Map!=v.type()){
			return Status("Resolution mismatch for '"+part+"'");
		}
		pointer[part]=v;
	}
	pointer[l]=data;
	return Status();
}


QString Key::toString() const
{
	return "store::Key{"+mStr+"}";
}


Key::operator QString() const
{
	return toString();
}


Context::Context()
{

}


void Context::get(Key key, std::function<void (Status status, QVariant data)> cb)
{
	QVariant data=key.lookUp(mData);
	cb(Status(), data);
}


void Context::put(Key key, std::function<void (Status status)> cb, QVariant data)
{
	auto s=key.put(mData, data);
	cb(s);
}


QString vtos(QVariant v){
	QString ret;
	switch(v.type()){
		case(QVariant::Map):{
			auto m=v.toMap();
			ret+="map("+QString::number(m.size())+"){\n";
			for(auto it= m.cbegin(), e=m.cend(); it != e; ++it) {
				ret+=QString("'%1': %2,").arg(it.key()).arg(vtos(it.value()));
			}
			ret+="}\n";
		}break;
		case(QVariant::List):{
			auto l=v.toList();
			const auto sz=l.size();
			ret+="list("+QString::number(sz)+"){\n";
			for(int i=0;i<sz;++i) {
				ret+=QString("[%1]=%2,").arg(i).arg(vtos(l[i]));
			}
			ret+="}\n";
		}break;
		default:
			ret+=v.toString();
	}
	return ret;
}

QString Context::toString() const
{
	return "store::Context{"+vtos(mData)+"}";
}


Context::operator QString() const
{
	return toString();
}


}

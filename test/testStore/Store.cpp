#include "Store.hpp"

#include <QDebug>
#include <QStack>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>


namespace  store{

QString KeyTokenTypeToString(KeyTokenType type)
{
#define KEYTOKENTYPETOSTRING_CASE(A) case(A): return #A
    switch(type){
    KEYTOKENTYPETOSTRING_CASE(NONE);
    KEYTOKENTYPETOSTRING_CASE(ERROR);
    KEYTOKENTYPETOSTRING_CASE(DIRECTORY);
    KEYTOKENTYPETOSTRING_CASE(ITEM);
    KEYTOKENTYPETOSTRING_CASE(PROPERTY);
    }
#undef KEYTOKENTYPETOSTRING_CASE
    return "UNKNOWN";
}

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


void Key::parse(){
    KeyParser kp(mStr);
    mPath=kp.parse();
}


bool KeyToken::valid() const{
    return ((!name.isEmpty()) && (NONE!=type));
}


QString KeyToken::toString() const
{
    return QString("KeyToken{type=%1, name=%2, nameIndex=%3, itemIndex=%4}")
            .arg(KeyTokenTypeToString(type)).arg(name).arg(nameIndex).arg(itemIndex);
}


KeyToken::operator QString() const
{
    return toString();
}


KeyToken::operator QString()
{
    return toString();
}


KeyParser::KeyParser(QString str)
    : last(NONE, "", 0)
    , index(0)
    , str(str)
{

}


KeyToken KeyParser::parsePart(){
    if(index<0){
        return KeyToken(NONE, "", 0);
    }
    bool first=true;
    KeyToken out(PROPERTY, "", index);
    const auto sz=str.size();
    while(true){
        if(index>=sz){
            return out;
        }
        QChar c=str[index];
        if(c.isLetter()){
            out.name+=c;
        }
        else{
            if(first){
                switch(c.toLatin1()){
                case('.'):out.type=PROPERTY; break;
                case('/'):out.type=DIRECTORY; break;
                default:
                    out.name="Unrecognized character: "+QString(c);
                    out.type=ERROR;
                    return out;
                }
            }
            // We are at end of current token
            else{
                return out;
            }
        }
        index++;
        first=false;
    }
}


QList<KeyToken> KeyParser::parse(){
    QList<KeyToken> out;
    while(true){
        KeyToken kt=parsePart();
        if(!kt.valid()){
            break;
        }
        out << kt;
    }
    return out;
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
        /*
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
        */

        return false;

    }


};


QVariant Key::lookUp(QVariant &root) const
{
    QVariant &pointer=root;
    for(auto part:mPath){
        qDebug().nospace().noquote()<<"Looking up part "<<part.name<<"("<<KeyTokenTypeToString(part.type)<<")";
        const auto ptype=pointer.type();
        switch(part.type){
        case(PROPERTY):{
            if(QVariant::Map == ptype){
                /*
                QVariantMap &map=qvariant_cast<QVariantMap &>(pointer);
                auto it=map.find(part.name);
                if(map.end()!=it){
                    pointer=it.value();
                }
                else{
                    // We lost track, no data here
                    return QVariant();
                }
                */
            }
            else{
                // return Status(QString("At traversal; Key type mismatch; %1 vs. %2 ").arg(QString::number(ptype)).arg(QString::number(QVariant::Map)));
                return QVariant();
            }
        }break;
        }
    }
    return pointer;
}


/*
if (variant.canConvert<QVariantList>()) {
    QSequentialIterable iterable = variant.value<QSequentialIterable>();
    // Can use foreach:
    foreach (const QVariant &v, iterable) {
        qDebug() << v;
    }
    // Can use C++11 range-for:
    for (const QVariant &v : iterable) {
        qDebug() << v;
    }
    // Can use iterators:
    QSequentialIterable::const_iterator it = iterable.begin();
    const QSequentialIterable::const_iterator end = iterable.end();
    for ( ; it != end; ++it) {
        qDebug() << *it;
    }
}




                QAssociativeIterable map = pointer->value<QAssociativeIterable>();
                QAssociativeIterable::const_iterator it=map.find(part.name);
                if(map.end()!=it){
                    auto lol=*it;
                    pointer=lol;
                }
                else{
                    map[part.name]=QVariantMap();
                    it=map.find(part.name);
                    if(map.end()!=it){
                        pointer=it.value();
                    }
                    else{
                        return Status(QString("Failed to insert default for key %1").arg(part.name));
                    }
                }

*/

/*
QVariantMap *map=qvariant_cast<QVariantMap *>(pointer);
auto it=map->find(part.name);
if(map->end()!=it){
    pointer=&it.value();
}
else{
    map[part.name]=QVariantMap();
    it=map.find(part.name);
    if(map.end()!=it){
        pointer=it.value();
    }
    else{
        return Status(QString("Failed to insert default for key %1").arg(part.name));
    }
}


 QVariant &v = map["some key"]; Q_ASSERT(!v.isNull()); Q_ASSERT(v.userType() == QMetaType::VariantMap); QVariantMap &innerMap = *reinterpret_cast<QVariantMap *>(v.data()); innerMap.mutate();


*/

Status Key::put(QVariant &root, QVariant data)
{
    QVariant *pointer=&root;
    auto w=mPath.mid(0, mPath.size()-1);
    auto l=mPath.last();
    qDebug()<<"W="<<w;
    qDebug()<<"L="<<l;
    for(auto part:w){
        qDebug()<<"STEP: "<<vtos(root);
        qDebug().nospace().noquote()<<"Traversing part "<<part.name<<"("<<KeyTokenTypeToString(part.type)<<")";
        QVariant v;
        const auto ptype=pointer->type();
        switch(part.type){
        case(PROPERTY):{
            if(QVariant::Map == ptype){
                QVariantMap &map = *reinterpret_cast<QVariantMap *>(pointer->data());
                auto it=map.find(part.name);
                if(map.end()!=it){
                    pointer=&*it;
                }
                else{
                    map[part.name]=QVariantMap();
                    auto it=map.find(part.name);
                    if(map.end()!=it){
                        pointer=&*it;
                    }
                    else{
                        return Status(QString("Failed to insert default for key %1").arg(part.name));
                    }
                }
            }
            else{
                return Status(QString("At traversal; Key type mismatch; %1 vs. %2 ").arg(QString::number(ptype)).arg(QString::number(QVariant::Map)));
            }
        }break;
        }
    }

    const auto ptype=pointer->type();
    if(QVariant::Map == ptype){
        QVariantMap &map = *reinterpret_cast<QVariantMap *>(pointer->data());
        switch(l.type){
        case(PROPERTY):{
            map[l.name]=data;
        }break;
        }
    }
    else{
        return Status(QString("At insertion; Key type mismatch; %1 vs. %2 ").arg(QString::number(ptype)).arg(QString::number(QVariant::Map)));
    }
    qDebug()<<"DONE: "<<vtos(root);
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


Key::operator QString()
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
    auto j=QJsonDocument::fromVariant(v);
    ret=j.toJson(QJsonDocument::Indented);
    /*
    switch(v.type()){
    case(QVariant::Map):{
        auto m=v.toMap();
        ret+="map("+QString::number(m.size())+"){";
        for(auto it= m.cbegin(), e=m.cend(); it != e; ++it) {
            ret+=QString("'%1': %2,").arg(it.key()).arg(vtos(it.value()));
        }
        ret+="}";
    }break;
    case(QVariant::List):{
        auto l=v.toList();
        const auto sz=l.size();
        ret+="list("+QString::number(sz)+"){";
        for(int i=0;i<sz;++i) {
            ret+=QString("[%1]=%2,").arg(i).arg(vtos(l[i]));
        }
        ret+="}";
    }break;
    default:
        ret+=v.toString();
    }
    */
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


Context::operator QString()
{
    return toString();
}


}

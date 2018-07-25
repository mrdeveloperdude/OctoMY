/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#ifndef KEYSTORE_HPP
#define KEYSTORE_HPP


#include "Key.hpp"
#include "PortableID.hpp"
#include "basic/GenerateRunnable.hpp"
#include "basic/AtomicBoolean.hpp"
#include "node/JsonAsyncBackend.hpp"
#include "node/AsyncStore.hpp"
#include "KeySecurityPolicy.hpp"


#include <QByteArray>
#include <QCryptographicHash>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QMutex>
#include <QSharedPointer>
#include <QString>

template <typename T>
static void noop(ASEvent<T> &)
{

}

struct nop {
    void operator()(...) const volatile {}
};


class KeyStore: public QObject, public AsyncFrontend<QVariantMap>
{
    Q_OBJECT
private:
    JsonAsyncBackend mBackend;
    AsyncStore<QVariantMap> mStore;
    QVariantMap mCache;
    bool mDirty;
    bool mDoBootstrap;
    KeySecurityPolicy mPolicy;
    QSharedPointer<Key> mLocalKey;
    QMap<QString, QSharedPointer<Key> > mAssociates;

    friend class GenerateRunnable<KeyStore>;

public:
    explicit KeyStore(QString filename="", bool doBootstrap=false, KeySecurityPolicy policy=KeySecurityPolicy(), QObject *parent=nullptr);
    virtual ~KeyStore();


    // AsyncFrontend interface
public:

    bool clearFrontend() Q_DECL_OVERRIDE;
    bool setFrontend(QVariantMap data) Q_DECL_OVERRIDE;
    QVariantMap getFrontend(bool &ok) Q_DECL_OVERRIDE;
    bool generateFrontend() Q_DECL_OVERRIDE;



private:

    // To have this called, make sure to set boostrapping to true and then call synchronize()
    void bootstrap();
public:

    bool bootstrapEnabled();
    void setBootstrapEnabled(bool doBootstrap);

    QSharedPointer<Key> localKey();


    // Check if we have pub-key for node identified by give fingerprint ID
    bool hasPubKeyForID(const QString &id);

    // Set pub-key for node identified by given fingerprint ID to given UTF-8 encoded string containing pubkey PEM format
    void setPubKeyForID(const QString &pubkeyPEM);

    // return pub-key for node identified by give fingerprint ID
    QSharedPointer<Key> pubKeyForID(const QString &id);



    void dump();
    QString toString();


public:

    QString filename() const;
    bool fileExists() const;
    bool ready();


    template <typename F>
    void status(F callBack);
    template <typename F>
    void clear(F callBack);
    template <typename F>
    void save(F callBack);
    template <typename F>
    void load(F callBack);
    template <typename F>
    void synchronize(F callBack);

    void waitForSync();

public:
    void clear()
    {
        clear([](ASEvent<QVariantMap> &ase){Q_UNUSED(ase);});
    }
    void save()
    {
        save([](ASEvent<QVariantMap> &ase){Q_UNUSED(ase);});
    }
    void load()
    {
        load([](ASEvent<QVariantMap> &ase){Q_UNUSED(ase);});
    }


public:

    friend const QDebug &operator<<(QDebug &d, KeyStore &ks);


signals:

    void keystoreReady(bool);
    //storeReady(!mError);

};






////////////////////////////////////////////////////////////////////////////////

template <typename F>
void KeyStore::status(F callBack)
{
    OC_METHODGATE();
    mStore.status().onFinished(callBack);
}

template <typename F>
void KeyStore::clear(F callBack)
{
    OC_METHODGATE();
    mStore.clear().onFinished(callBack);
}

template <typename F>
void KeyStore::save(F callBack)
{
    OC_METHODGATE();
    mStore.save().onFinished(callBack);
}



template <typename F>
void KeyStore::load(F callBack)
{
    OC_METHODGATE();
    mStore.load().onFinished(callBack);
}



template <typename F>
void KeyStore::synchronize(F callBack)
{
    OC_METHODGATE();
    mStore.synchronize().onFinished(callBack);
}




const QDebug &operator<<(QDebug &d, KeyStore &ks);




#endif // KEYSTORE_HPP

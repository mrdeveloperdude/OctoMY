/* This file is distributed under the license as specified in the LICENSE file
 * that accompanied it. If there was no LICENSE file accompanying this source,
 * it is not to be modified, redistributed or used without prior written
 * permission from original author and owner "Lennart Rolland".
 */

#include "KeyStore.hpp"
#include "../libutil/utility/Utility.hpp"

#include "SecurityConstants.hpp"

#include <QJsonObject>
#include <QJsonParseError>
#include <QFile>
#include <QRunnable>
#include <QThreadPool>


/////////////////////////////////////////////////////////////////////////////


KeyStore::KeyStore(QString fn, QObject *parent)
	: AsyncStore(fn, parent)
{
	setObjectName("KeyStore");
//qDebug()<<"KeyStore() file="<<fn;
	if(!connect( this, SIGNAL(storeReady(bool)), SIGNAL(keystoreReady(bool)), OC_CONTYPE)) {
		qWarning()<<"Could not connect "<<objectName();
	} else {
		//qDebug()<<"FORWARDING storeReady -> keystoreReady";
	}
}


KeyStore::~KeyStore()
{
	save();
}


void KeyStore::bootstrapWorkerImpl()
{
	//qDebug()<<"KeyStore() bootstrapWorkerImpl() file="<<mFilename;
	QFile f(mFilename);
	if(!f.exists()) {
		qDebug()<<"KEYSTORE: no keystore file found, generating local keypair and saving";
		mLocalKey=Key(OCTOMY_KEY_BITS);
		save();
	}
	load();
}


void KeyStore::load()
{
	//qDebug()<<"KeyStore() load="<<mFilename;
	if(!QFile(mFilename).exists()) {
		//qDebug() << "File did not exist"<<mFilename;
		//Silently ignore missing file
		//mError=true;
	} else {
		//qDebug()<<"KEYSTORE: Loading from file: " << *this;
		QJsonParseError jsonError;
		QByteArray raw=utility::fileToByteArray(mFilename);
		QJsonDocument doc = QJsonDocument::fromJson(raw, &jsonError);
		if (QJsonParseError::NoError != jsonError.error) {
			qWarning() << "ERROR: Parsing json data: "<<jsonError.errorString()<< " for data "<<raw<<" from file "<<mFilename;
			mError=true;
		} else {
			//qDebug()<<"PARSING JSON: "<<doc.toJson();
			QVariantMap map = doc.object().toVariantMap();
			mLocalKey=Key(map["localKey"].toMap(),false);
			if(!mLocalKey.isValid(false)) {
				qWarning()<<"ERROR: local key was not valid";
				mError=true;
			} else {

				QVariantList remotes=map["remoteKeys"].toList();
				mPeers.clear();
				for(QVariantList::iterator b=remotes.begin(), e=remotes.end(); b!=e; ++b) {
					QVariantMap remote=(*b).toMap();
					Key peerKey(remote,true);
					if(!peerKey.isValid(true)) {
						qWarning()<<"ERROR: peerk key was not valid";
						mError=true;
						break;
					}
					mPeers[remote["id"].toString()]=peerKey;
				}
			}
			mReady=true;
			//qDebug()<<"RESULT AFTER LOAD: "<<this;
		}
	}
	//qDebug()<<"EMITTING KEYSTORE READY WITH"<<(mError?"ERROR":"A-OK");
	emit storeReady(!mError);
}

void KeyStore::save()
{
	//qDebug()<<"KeyStore() save="<<mFilename;
	//qDebug()<<"KEYSTORE: Saving to file: "<<*this;
	QVariantMap map;
	map["createdTimeStamp"]=QDateTime::currentMSecsSinceEpoch();
	map["localKey"]=mLocalKey.toVariantMap(false);
	QVariantList remotes;
	for(QMap<QString, Key >::iterator b=mPeers.begin(), e=mPeers.end(); b!=e; ++b) {
		QVariantMap remote;
		remote["id"]=b.key();
		remote["key"]=b.value().toVariantMap(true);
		remotes.push_back(remote);
	}
	map["remoteKeys"]=remotes;
	QJsonDocument doc=QJsonDocument::fromVariant(map);
	QString raw=doc.toJson();
	//qDebug()<<"SAVING JSON: "<<raw;
	utility::stringToFile(mFilename,raw);
}


void KeyStore::clear()
{
	//qDebug()<<"KeyStore() clear="<<mFilename;
	QFile file(mFilename);
	if(file.exists()) {
		if(file.remove()) {
			//qDebug()<<"KEYSTORE: Cleared: "<<*this;
			mLocalKey=Key();
			mPeers.clear();
			mReady=false;
			mError=false;
		} else {
			qWarning()<<"ERROR: Could not clear "<<*this;
		}
	} else {
		//qDebug()<<"KEYSTORE: Could not clear missing file: "<< *this;
	}

}


void KeyStore::hookSignals(QObject &ob)
{
	if(!connect(this, SIGNAL(keystoreReady(bool)), &ob, SLOT(onKeystoreReady(bool)),OC_CONTYPE)) {
		qWarning()<<"Could not connect "<<ob.objectName();
	} else {
		//qDebug()<<"HOOKING keystoreReady";
	}
}

void KeyStore::unhookSignals(QObject &ob)
{
	if(!disconnect(this, SIGNAL(keystoreReady(bool)), &ob, SLOT(onKeystoreReady(bool)))) {
		qWarning()<<"Could not disconnect "<<ob.objectName();
	} else {
		//qDebug()<<"UN-HOOKING keystoreReady";
	}
}


QByteArray KeyStore::sign(const QByteArray &source)
{
	if(!mReady) {
		return QByteArray();
	}
	return mLocalKey.sign(source);
}


bool KeyStore::verify(const QByteArray &message, const QByteArray &signature)
{
	if(!mReady) {
		return false;
	}
	return mLocalKey.verify(message, signature);
}


bool KeyStore::verify(const QString &fingerprint, const QByteArray &message, const QByteArray &signature)
{
	if(!mReady) {
		return false;
	}
	QMap<QString, Key >::iterator f=mPeers.find(fingerprint);
	if(mPeers.end()==f) {
		return false;
	}
	Key remote=f.value();
	return remote.verify(message, signature);
}


bool KeyStore::hasPubKeyForID(const QString &id)
{
	if(!mReady) {
		return false;
	}
	return (mPeers.end()==mPeers.find(id));
}

void KeyStore::setPubKeyForID(const QString &pubkeyPEM)
{
	if(!mReady) {
		return;
	}
	Key peer(pubkeyPEM, true);
	mPeers[peer.id()]=peer;
}

Key KeyStore::pubKeyForID(const QString &id)
{
	if(!mReady) {
		return Key();
	}
	mPeers[id];
}

const QDebug &operator<<(QDebug &d, KeyStore &ks)
{
	d.nospace() <<"fn="<<ks.mFilename<<", fexists="<<ks.fileExists()<<", ready="<<(const bool)ks.mReady<<", inProgress="<<(const bool)ks.mInProgress<<", error="<<(const bool)ks.mError;
	return d.maybeSpace();
}





/*
 *
void HubWindow::on_pushButtonTestKeyPair_clicked(){
	//const auto priPath = QString("/home/lennart/keypairs/private_key.pem"); 	const auto pubPath = QString("/home/lennart/keypairs/public_key.pem"); utility::fileToByteArray(pubPath);
	int ret=0;
	qpolarssl::Pki pkiPrivate;
	QByteArray priKeyData  = ui->plainTextEditPrivateKey->toPlainText().toUtf8();
	if( (ret=pkiPrivate.parseKey(priKeyData) != 0) ){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText("Parsing of private key failed with code "+QString::number(ret));
		msgBox.exec();
		return;
	}
	else if(!pkiPrivate.isValid()){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText("PKI was invalid after parsing of private key");
		msgBox.exec();
		return;
	}

	qpolarssl::Pki pkiPublic;
	QByteArray pubKeyData  = ui->plainTextEditPublicKey->toPlainText().toUtf8();
	if( (ret=pkiPublic.parsePublicKey(pubKeyData) != 0) ){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText("Parsing of public key failed with code -"+QString::number(-ret,16));
		msgBox.exec();
		return;
	}
	else if(!pkiPublic.isValid()){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText("PKI was invalid after parsing of public key");
		msgBox.exec();
		return;
	}

	const QByteArray sourceData = QString("OctoMY™ test data in cleartext").toUtf8();

	const QByteArray signature = pkiPrivate.sign(sourceData, qpolarssl::THash::SHA256);

	const int siglen=signature.length();
	if((siglen <= 64) ){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText("Signature size "+QString::number(siglen)+" was less than 64");
		msgBox.exec();
		return;
	}

	ret = pkiPublic.verify(sourceData, signature, qpolarssl::THash::SHA256);

	if(ret!=0){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText("Signature verification failed with code -"+QString::number(-ret,16));
		msgBox.exec();
		return;
	}

	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Information);
	msgBox.setText("Signature verification succeeded.");
	msgBox.exec();
}




void KeyStore::verifyKeypair(){
	//const auto priPath = QString("/home/lennart/keypairs/private_key.pem"); 	const auto pubPath = QString("/home/lennart/keypairs/public_key.pem"); utility::fileToByteArray(pubPath);
	int ret=0;
	qpolarssl::Pki pkiPrivate;
	QByteArray priKeyData  = ui->plainTextEditPrivateKey->toPlainText().toUtf8();
	if( (ret=pkiPrivate.parseKey(priKeyData) != 0) ){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText("Parsing of private key failed with code "+QString::number(ret));
		msgBox.exec();
		return;
	}
	else if(!pkiPrivate.isValid()){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText("PKI was invalid after parsing of private key");
		msgBox.exec();
		return;
	}

	qpolarssl::Pki pkiPublic;
	QByteArray pubKeyData  = ui->plainTextEditPublicKey->toPlainText().toUtf8();
	if( (ret=pkiPublic.parsePublicKey(pubKeyData) != 0) ){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText("Parsing of public key failed with code -"+QString::number(-ret,16));
		msgBox.exec();
		return;
	}
	else if(!pkiPublic.isValid()){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText("PKI was invalid after parsing of public key");
		msgBox.exec();
		return;
	}

	const QByteArray sourceData = QString("OctoMY™ test data in cleartext").toUtf8();

	const QByteArray signature = pkiPrivate.sign(sourceData, qpolarssl::THash::SHA256);

	const int siglen=signature.length();
	if((siglen <= 64) ){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText("Signature size "+QString::number(siglen)+" was less than 64");
		msgBox.exec();
		return;
	}

	ret = pkiPublic.verify(sourceData, signature, qpolarssl::THash::SHA256);

	if(ret!=0){
		QMessageBox msgBox;
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setText("Signature verification failed with code -"+QString::number(-ret,16));
		msgBox.exec();
		return;
	}

	QMessageBox msgBox;
	msgBox.setIcon(QMessageBox::Information);
	msgBox.setText("Signature verification succeeded.");
	msgBox.exec();
}
*/

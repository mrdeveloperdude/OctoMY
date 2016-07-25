#include "KeyStore.hpp"
#include "utility/Utility.hpp"

#include "SecurityConstants.hpp"

#include <QJsonObject>
#include <QJsonParseError>
#include <QFile>
#include <QRunnable>
#include <QThreadPool>



/////////////////////////////////////////////////////////////////////////////




KeyStore::KeyStore(QString fn, QObject *parent)
	: QObject(parent)
	, mReady(false)
	, mError(false)
	, mInProgress(false)
	, mFilename(fn)
{

}



KeyStore::~KeyStore()
{
	save();
}


void KeyStore::bootstrap(bool loadOnly, bool runInBackground)
{
	//qDebug()<<"Keystore bootstrap() loadOnly="<<loadOnly<<", bg="<<runInBackground<<", "<<*this;
	if(mReady){
		emit keystoreReady(!mError);
		return;
	}
	else if(mInProgress){
		return;
	}
	else if(loadOnly){
		if(QFile(mFilename).exists()){
			load();
		}
		return;
	}
	// QThreadPool takes ownership and deletes runnable automatically after completion
	else if(runInBackground){
		QThreadPool *tp=QThreadPool::globalInstance();
		if(nullptr!=tp){
			const bool ret=tp->tryStart(new GenerateRunnable<KeyStore>(*this));
			if(ret){
				//qDebug()<<"KEYSTORE: Successfully started background thread";
				return;
			}
		}
		else{
			qWarning()<<"ERROR: No global threadpool available, defaulting to serial version";
		}
	}
	else {
		// Fall back to single threaded wday
		qDebug()<<"KEYSTORE: Falling back to serial bootstrap";
		bootstrapWorker();
	}
}


void KeyStore::bootstrapWorker()
{
	mInProgress=true;
	QFile f(mFilename);
	if(!f.exists()){
		qDebug()<<"KEYSTORE: no keystore file found, generating local keypair and saving";
		mLocalKey=Key(OCTOMY_KEY_BITS);
		save();
	}
	load();
	mInProgress=false;
}


void KeyStore::load()
{
	//qDebug()<<"KEYSTORE: Loading from file: " << *this;
	QJsonParseError jsonError;
	QByteArray raw=utility::fileToByteArray(mFilename);
	QJsonDocument doc = QJsonDocument::fromJson(raw, &jsonError);
	if (QJsonParseError::NoError != jsonError.error){
		qWarning() << "ERROR: Parsing json data: "<<jsonError.errorString()<< " for data "<<raw<<" from file "<<mFilename;
		mError=true;
	}
	else{
		//qDebug()<<"PARSING JSON: "<<doc.toJson();
		QVariantMap map = doc.object().toVariantMap();
		mLocalKey=Key(map["localKey"].toMap(),false);
		mError=mError||(!mLocalKey.isValid(false));
		QVariantList remotes=map["remoteKeys"].toList();
		mPeers.clear();
		for(QVariantList::iterator b=remotes.begin(), e=remotes.end(); b!=e; ++b){
			QVariantMap remote=(*b).toMap();
			mPeers[remote["id"].toString()]=Key(remote,true);
		}
		mReady=true;
		//qDebug()<<"RESULT AFTER LOAD: "<<this;
	}
	//qDebug()<<"EMITTING KEYSTORE READY WITH"<<(mError?"ERROR":"A-OK");
	emit keystoreReady(!mError);
}

void KeyStore::save()
{
	//qDebug()<<"KEYSTORE: Saving to file: "<<*this;
	QVariantMap map;
	map["createdTimeStamp"]=QDateTime::currentMSecsSinceEpoch();
	map["localKey"]=mLocalKey.toVariantMap(false);
	QVariantList remotes;
	for(QMap<QString, Key >::iterator b=mPeers.begin(), e=mPeers.end(); b!=e; ++b){
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
	QFile file(mFilename);
	if(file.exists()){
		if(file.remove()){
			qDebug()<<"KEYSTORE: Cleared: "<<*this;
			mLocalKey=Key();
			mPeers.clear();
			mReady=false;
			mError=false;
		}
		else{
			qWarning()<<"ERROR: Could not clear "<<*this;
		}
	}
	else{
		qDebug()<<"KEYSTORE: Could not clear missing file: "<< *this;
	}

}



QByteArray KeyStore::sign(const QByteArray &source)
{
	if(!mReady){
		return QByteArray();
	}
	return mLocalKey.sign(source);
}


bool KeyStore::verify(const QByteArray &message, const QByteArray &signature)
{
	if(!mReady){
		return false;
	}
	return mLocalKey.verify(message, signature);
}


bool KeyStore::verify(const QString &fingerprint, const QByteArray &message, const QByteArray &signature)
{
	if(!mReady){
		return false;
	}
	QMap<QString, Key >::iterator f=mPeers.find(fingerprint);
	if(mPeers.end()==f){
		return false;
	}
	Key remote=f.value();
	return remote.verify(message, signature);
}


bool KeyStore::hasPubKeyForFingerprint(const QString &fingerprint)
{
	if(!mReady){
		return false;
	}
	return (mPeers.end()==mPeers.find(fingerprint));
}

void KeyStore::setPubKeyForFingerprint(const QString &pubkeyPEM)
{
	if(!mReady){
		return;
	}
	Key peer(pubkeyPEM, true);
	mPeers[peer.id()]=peer;
}



const QDebug &operator<<(QDebug &d, KeyStore &ks)
{
	d.nospace() <<"fn="<<ks.mFilename<<", fexists="<<ks.hasLocalKeyFile()<<", ready="<<(const bool)ks.mReady<<", inProgress="<<(const bool)ks.mInProgress<<", error="<<(const bool)ks.mError;
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

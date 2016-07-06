#include "KeyStore.hpp"
#include "utility/Utility.hpp"

#include "SecurityConstants.hpp"

#include <QJsonObject>
#include <QJsonParseError>
#include <QFile>
#include <QRunnable>
#include <QThreadPool>



/////////////////////////////////////////////////////////////////////////////




KeyStore::KeyStore(QObject *parent, QString fn)
	: QObject(parent)
	, ready(false)
	, error(false)
	, filename(fn)
{
	if(QFile(filename).exists()){
		load();
	}
}



KeyStore::~KeyStore(){
	save();
}


void KeyStore::bootstrap(){
	// QThreadPool takes ownership and deletes runnable automatically after completion
	QThreadPool *tp=QThreadPool::globalInstance();
	if(0!=tp){
		const bool ret=tp->tryStart(new GenerateRunnable<KeyStore>(*this));
		if(ret){
			//qDebug()<<"KEYSTORE: Successfully started background thread";
			return;
		}
	}
	// Fall back to single threaded wday
	qDebug()<<"KEYSTORE: Falling back to serial bootstrap";
	bootstrapWorker();
}


void KeyStore::bootstrapWorker(){
	QFile f(filename);
	if(!f.exists()){
		qDebug()<<"KEYSTORE: no keystore file found, generating local keypair and saving";
		localKey=Key(OCTOMY_KEY_BITS);
		//local_pki.reset();		local_pki.generateKeyPair(keyBits);
		save();
	}
	load();
}


void KeyStore::load(){
	//qDebug()<<"KEYSTORE: Loading from file";
	QJsonParseError jsonError;
	QByteArray raw=utility::fileToByteArray(filename);
	QJsonDocument doc = QJsonDocument::fromJson(raw, &jsonError);
	if (QJsonParseError::NoError != jsonError.error){
		qWarning() << "ERROR: Parsing json data: "<<jsonError.errorString()<< " for data "<<raw<<" from file "<<filename;
		error=true;
	}
	else{
		//qDebug()<<"PARSED JSON: "<<doc.toJson();
		QVariantMap map = doc.object().toVariantMap();
		localKey=Key(map,false);
		/*
		local_pki.reset();
		local_pki.parseKey(map["localPrivateKey"].toByteArray());
		local_pki.parsePublicKey(map["localPublicKey"].toByteArray());
		*/
		QVariantList remotes=map["remoteKeys"].toList();
		peers.clear();
		for(QVariantList::iterator b=remotes.begin(), e=remotes.end(); b!=e; ++b){
			QVariantMap remote=(*b).toMap();
			peers[remote["id"].toString()]=Key(remote,true);
		}
		ready=true;
	}
	emit keystoreReady();
}

void KeyStore::save(){
	qDebug()<<"KEYSTORE: Saving to file: "<<filename;
	QVariantMap map;
	map["createdTimeStamp"]=QDateTime::currentMSecsSinceEpoch();
	map["localKey"]=localKey.toVariantMap();
	QVariantList remotes;
	for(QMap<QString, Key >::iterator b=peers.begin(), e=peers.end(); b!=e; ++b){
		QVariantMap remote;
		remote["id"]=b.key();
		remote["key"]=b.value().toVariantMap();
		remotes.push_back(remote);
	}
	map["remoteKeys"]=remotes;
	QJsonDocument doc=QJsonDocument::fromVariant(map);
	utility::stringToFile(filename,doc.toJson());
}

void KeyStore::clear(){
	QFile file(filename);
	if(file.exists()){
		if(file.remove()){
			qDebug()<<"KEYSTORE: Cleared: "<<filename;
			localKey=Key();
			ready=false;
			error=false;
		}
		else{
			qWarning()<<"ERROR: Could not clear "<<filename;
		}
	}
	else{
		qDebug()<<"KEYSTORE: Could not clear "<<filename<<" did not exist";
	}

}



QByteArray KeyStore::sign(const QByteArray &source){
	if(!ready){
		return QByteArray();
	}
	return localKey.sign(source);
}


bool KeyStore::verify(const QByteArray &message, const QByteArray &signature){
	if(!ready){
		return false;
	}
	return localKey.verify(message, signature);
}


bool KeyStore::verify(const QString &fingerprint, const QByteArray &message, const QByteArray &signature){
	if(!ready){
		return false;
	}
	QMap<QString, Key >::iterator f=peers.find(fingerprint);
	if(peers.end()==f){
		return false;
	}
	Key remote=f.value();
	return remote.verify(message, signature);
}


bool KeyStore::hasPubKeyForFingerprint(const QString &fingerprint){
	if(!ready){
		return false;
	}
	return (peers.end()==peers.find(fingerprint));
}

void KeyStore::setPubKeyForFingerprint(const QString &pubkeyPEM){
	if(!ready){
		return;
	}
	Key peer(pubkeyPEM, true);
	peers[peer.id()]=peer;
}


Key KeyStore::getLocalKey(){
	return localKey;
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

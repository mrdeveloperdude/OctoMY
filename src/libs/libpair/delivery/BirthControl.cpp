#include "BirthControl.hpp"

#include "node/Node.hpp"
#include "security/KeyStore.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "utility/time/HumanTime.hpp"

const quint64 BirthControl::MINIMUM_BIRTH_TIME{3000};


BirthControl::BirthControl(QObject *parent)
	: QObject(parent)
	, mConfigureHelper("BirthControl", true, false, false, true, false)
{
	OC_METHODGATE();
}


void BirthControl::configureTimer(){
	if(mConfigureHelper.isConfiguredAsExpected()) {
		mBirthTimer.setInterval(MINIMUM_BIRTH_TIME); //Minimum birth time gives this moment some depth in case keygen should finish quickly.
		mBirthTimer.setSingleShot(true);
		
		if(!connect(&mBirthTimer, &QTimer::timeout, this, [this]() {
				mTimerCompleted = true;
				qDebug()<<"Birth timer timed out, calling birth done";
				onBirthComplete(false, "Timed out");
			}, OC_CONTYPE_NON_UNIQUE)) {
			qWarning()<<"ERROR: Could not connect";
		}
	}
	
}

void BirthControl::configure(QSharedPointer <Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode = node;
		configureTimer();
	}
}


void BirthControl::birth(const QString &name, const QString &gender){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mNode.isNull()) {
			auto keystore = mNode->keyStore();
			if(!keystore.isNull()) {
				mDoneMessage = "TIMER ERROR";
				mDoneStatus = false;
				mTimerCompleted = false;
				mKeygenCompleted = false;
				mBirthTimer.start();
				emit birthProgress("Birth started", 1, 2);
				QTimer::singleShot(0, Qt::VeryCoarseTimer, [=]() {
					keystore->clear([=](ASEvent<QVariantMap> &ase1) {
						Q_UNUSED(ase1);
						const auto old = keystore->bootstrapEnabled();
						keystore->setBootstrapEnabled(true);
						keystore->clear();
						keystore->save();
						keystore->synchronize([=](ASEvent<QVariantMap> &ase2) {
							const bool ok = ase2.isSuccessfull();
							if(!ok){
								mKeygenCompleted = true;
								onBirthComplete(false, "Key generation failed");
							}
							else{
								auto key = keystore->localKey();
								if(!key.isNull()) {
									const auto birthDate = utility::time::currentMsecsSinceEpoch<quint64>();
									const auto actualName = name.isEmpty() ? mAgentNameGenerator.generate():name;
									const auto actualGender = gender.isEmpty() ? mGenderGenerator.generate():gender;
									QVariantMap map;
									map["id"] = key->id();
									map["name"] = actualName;
									map["gender"] = actualGender;
									map["role"] = nodeRoleToString(mNode->nodeRole());
									map["type"] = nodeTypeToString(mNode->nodeType());
									map["birthDate"] = utility::time::msToVariant(birthDate);
									qDebug() << "Creating new identity: " << map;
									mNode->setNodeIdentity(map);
									keystore->setBootstrapEnabled(old);
									mKeygenCompleted = true;
									onBirthComplete(true, "");
								}
								else {
									mKeygenCompleted = true;
									onBirthComplete(false, "No key");
								}
							}
						});
					});
				});
			} else {
				mKeygenCompleted = true;
				onBirthComplete(false, "No keystore");
			}
		} else {
			mKeygenCompleted = true;
			onBirthComplete(false, "No node");
		}
	}
	else{
		mKeygenCompleted = true;
		onBirthComplete(false, "Not configured");
	}
}


void BirthControl::onBirthComplete(bool ok, const QString &message){
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mTimerCompleted && mKeygenCompleted){
			mDoneMessage = message;
			mDoneStatus = ok;
		}
		if(mKeygenCompleted && mTimerCompleted){
			qDebug() << "BirthControl:onBirthProgress " << mDoneStatus << mDoneMessage;
			emit birthProgress("Birth completed", 2, 2);
			emit birthComplete(mDoneStatus, mDoneMessage);
			mBirthTimer.stop();
		}
	}
}


#ifdef old_stuff



void AgentDeliveryActivity::startBirth()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mNode.isNull()) {
			QString name = ui->lineEditName->text();
			name[0] = name[0].toUpper();
			mID.setName(name);
			GenderGenerator gg;
			QString gender = 0>ui->comboBoxGender->currentIndex()?ui->comboBoxGender->currentText():gg.generate();
			mID.setGender(gender);
			qDebug() << "XXX - Started birth of agent with NAME: " << name << ", GENDER: " << gender;
			mSpinner->start();
			mCompleteCounter = 0;
			mCompleteOK = false;
			mBirthTimer.start();
			ui->stackedWidget->setCurrentWidget(ui->pageBirthInProgress);
			QTimer::singleShot(0, Qt::VeryCoarseTimer, [=]() {
				QSharedPointer<KeyStore> keystore = mNode->keyStore();
				if(!keystore.isNull()) {
					mLastBootstrap = keystore->bootstrapEnabled();
					keystore->setBootstrapEnabled(true);
					keystore->clear();
					keystore->save();
					qDebug() << "Synchronizing keystore START";
					keystore->synchronize([this](ASEvent<QVariantMap> &se) {
						const bool ok = se.isSuccessfull();
						qDebug() << "synchronized keystore, calling birth done";
						onBirthComplete(ok);
					});
					qDebug()<<"Synchronizing keystore END";
				} else {
					qDebug()<<"ERROR: No keystore";
				}
			});
		}
	}
}




void AgentDeliveryActivity::onBirthComplete(bool ok, const QString &message)
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		qDebug() << "AgentDeliveryActivity:onBirthProgress " << ok << message;
		QMutexLocker timeoutLock(&mTimeoutMutex);
		if(!mNode.isNull()) {
			auto keystore = mNode->keyStore();
			if(!keystore.isNull()) {
				qDebug()<<"XXX - keystore.ready()="<<keystore->ready()<<", ok="<<ok;
				keystore->setBootstrapEnabled(mLastBootstrap);
				mCompleteOK |= ok;
				mCompleteCounter++;
				if(mCompleteCounter < 2) {
					qDebug()<<"XXX - Birth almost complete...";
				} else {
					
					qDebug()<<"XXX - Birth complete!";
					mBirthTimer.stop();
					mSpinner->stop();
					const bool dok = keystore->ready();
					if(!dok || !ok ) {
						qWarning() << "XXX - ERROR: Birthdefects detected: datastore.ready=" << dok << ", ok=" << ok;
						qDebug() << "XXX: DATA AFTER AFILED LOAD WAS: " << keystore;
						//Go back to try again
						ui->stackedWidget->setCurrentWidget(ui->pageDelivery);
					} else {
						mBirthDate = utility::time::currentMsecsSinceEpoch<quint64>();
						QVariantMap map;
						auto key = keystore->localKey();
						if(!key.isNull()) {
							//map["key"] = key->toVariantMap(true);
							map["name"] = ui->lineEditName->text();
							if(0 == ui->comboBoxGender->currentIndex()) {
								GenderGenerator gg;
								ui->comboBoxGender->setCurrentText(gg.generate());
							}
							map["gender"] = ui->comboBoxGender->currentText();
							map["type"] = nodeTypeToString(TYPE_AGENT);
							map["role"] = nodeRoleToString(ROLE_AGENT);
							map["birthDate"] = utility::time::msToVariant(mBirthDate);
							mNodeIdentity = QSharedPointer<Associate>::create(map);
							mNode->setNodeIdentity(mNodeIdentity);
							mID = mNodeIdentity->toPortableID();
							ui->widgetBirthCertificate->setPortableID(mID);
							ui->stackedWidget->setCurrentWidget(ui->pageDone);
							QString text = "My name is "+mID.name()+". I am an octomy agent. How do you do!";
							if(mUseVoice){
								VoiceManager::speak(mID, text);
							}
						} else {
							qWarning()<<"ERROR: No key";
						}
					}
				}
			} else {
				qWarning()<<"ERROR: No keystore";
			}
		} else {
			qWarning()<<"ERROR: No node";
		}
		qDebug()<<"onBirthComplete over with completeCounter="<<mCompleteCounter<<" and page="<<ui->stackedWidget->currentWidget()->objectName();
	}
}



#endif





void BirthControl::unBirth()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()) {
		if(!mNode.isNull()) {
			mNode->unbirth();
		} else {
			qWarning()<<"WARNING: Could not unbirth, no node";
		}
	}
}


DeliveryStatus BirthControl::statusAssesment(){
	OC_METHODGATE();
	auto localKey = mNode->keyStore()->localKey();
	auto nodeIdentity = mNode->nodeIdentity();
	DeliveryStatus status;
	status.localKey = localKey;
	status.nodeIdentity = nodeIdentity;
	status.hasLocalKey = !localKey.isNull();
	status.messages.clear();
	if(!status.hasLocalKey){
		status.messages << "No local key";
	}
	else{
		
		status.hasLocalKeyID = !localKey->id().isEmpty();
		if(!status.hasLocalKeyID){
			status.messages << "No ID in local key";
		}
		status.hasLocalKeyPublic = !localKey->pubKey().isEmpty();
		if (!status.hasLocalKeyPublic) {
			status.messages << "No public key in local key";
		}
		
		status.hasLocalKeyPrivate = !localKey->key().isEmpty();
		if (!status.hasLocalKeyPrivate) {
			status.messages << "No private key in local key";
		}
		
	}
	
	QStringList node_messages;
	if(!nodeIdentity.isNull()){
		
		status.hasNodeIdentityID = !nodeIdentity->id().isEmpty();
		if(!status.hasNodeIdentityID){
			node_messages << "No ID in node identity";
		}
		if(status.hasLocalKey){
			status.hasNodeIdentityIDMatchesLocalKeyID = nodeIdentity->id() == localKey->id();
			if(!status.hasNodeIdentityIDMatchesLocalKeyID){
				node_messages << "ID in node identity did not match ID in local key";
			}
		}
		status.hasNodeIdentityName = !nodeIdentity->name().isEmpty();
		if (!status.hasNodeIdentityName) {
			node_messages << "No name in node identity";
		}
		
		status.hasNodeIdentityGender = !nodeIdentity->gender().isEmpty();
		if (!status.hasNodeIdentityGender) {
			node_messages << "No gender in node identity";
		}
		
		status.hasNodeIdentityType = TYPE_UNKNOWN != nodeIdentity->type();
		if (!status.hasNodeIdentityType) {
			node_messages << "No type in node identity";
		}
		
		status.hasNodeIdentityRole = ROLE_UNKNOWN != nodeIdentity->role();
		if (!status.hasNodeIdentityRole) {
			node_messages << "No role in node identity";
		}
	}
	status.hasNodeIdentity =
		   status.hasNodeIdentityID
		|| status.hasNodeIdentityName
		|| status.hasNodeIdentityGender
		|| status.hasNodeIdentityType
		|| status.hasNodeIdentityRole;
	if(!status.hasNodeIdentity){
		status.messages << "No node identity";
	}
	else{
		status.messages << node_messages;
	}
	// If everything is valid, we could set a success message or just return the status as is.
	return status;
}


#ifndef PAIRINGMANAGER_HPP
#define PAIRINGMANAGER_HPP



#include <QTimer>
#include <QObject>
#include <QDateTime>

enum PairingRole{
	ROLE_AGENT
	, ROLE_CONTROL
};


class ZooClient;


const quint64 PAIRING_SESSION_EXIRY_TIME = 20 * 60 * 1000; // 20 minutes

class PairingSession{

	private:

		bool sheHasMyKey;
		bool iHaveHerKey;
		bool sheHasVerifiedMe;
		bool iHaveVerifiedHer;
		bool expired;
		quint64 lastUpdate;

	private:

		bool updateExpired(){
			if(!expired){
				const quint64 now=QDateTime::currentMSecsSinceEpoch();
				if(lastUpdate+PAIRING_SESSION_EXIRY_TIME>now){
					expired=true;
				}
				else{
					lastUpdate=now;
				}
			}
			return expired;
		}


	public:

		PairingSession()
			: sheHasMyKey(false)
			, iHaveHerKey(false)
			, sheHasVerifiedMe(false)
			, iHaveVerifiedHer(false)
			, expired(false)
			, lastUpdate(QDateTime::currentMSecsSinceEpoch())
		{

		}

		bool isComplete(){
			return !expired
					&& (
						sheHasMyKey
						&& iHaveHerKey
						&& sheHasVerifiedMe
						&& iHaveVerifiedHer
						)
					;

		}


		void setHerKey(){
			if(!iHaveHerKey && !updateExpired()){
				iHaveHerKey=true;
			}
		}
		void setHerVerification(){
			if(!iHaveVerifiedHer && !updateExpired()){
				iHaveVerifiedHer=true;
			}
		}

		void setMyKey(){
			if(!sheHasMyKey && !updateExpired()){
				sheHasMyKey=true;
			}
		}
		void setMyVerification(){
			if(!sheHasVerifiedMe && !updateExpired()){
				sheHasVerifiedMe=true;
			}
		}

};

class PairingManager: public QObject
{
		Q_OBJECT
	private:
		PairingRole role;
		QTimer timer;
		ZooClient *zoo;
		quint64 lastZooPair;
		PairingSession *zooSession;
	public:
		PairingManager(QObject *parent=0);

	public:

		void start();
		void stop();
		void setRole(PairingRole);

	private slots:

		void onTimer();
};

#endif // PAIRINGMANAGER_HPP

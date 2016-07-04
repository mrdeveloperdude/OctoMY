#ifndef DISCOVERYCLIENTSTORE_HPP
#define DISCOVERYCLIENTSTORE_HPP

#include "basic/AtomicBoolean.hpp"
#include "basic/GenerateRunnable.hpp"
#include "DiscoveryParticipant.hpp"

#include <QObject>


/*

  DiscoveryClientStore is used by Node to keep track of DiscoveryParticpants
  during discovery. It is also used to persist the information Node has on its
  particpants.

*/

class DiscoveryClientStore: public QObject{
		Q_OBJECT


	private:


		QMap<QString, DiscoveryParticipant *> peers;
		AtomicBoolean ready;
		AtomicBoolean error;
		QString filename;


		friend class GenerateRunnable<DiscoveryClientStore>;

	public:
		explicit DiscoveryClientStore(QObject *parent=0, QString="");
		virtual ~DiscoveryClientStore();

	private:
		void bootstrapWorker();
		void load();
		void save();


	public:

		//Make if needed, load if otherwise
		void bootstrap();

		bool isReady(){
			return ready;
		}
		bool isError(){
			return error;
		}


		bool hasParticipant(const QString &id);
		DiscoveryParticipant * getParticipant(const QString &id);
		void setParticipant(DiscoveryParticipant *participant);


		QMap<QString, DiscoveryParticipant *> &getParticipants();

	signals:

		void discoveryStoreReady();
};


#endif // DISCOVERYCLIENTSTORE_HPP

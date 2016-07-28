#ifndef NODEASSOCIATESTORE_HPP
#define NODEASSOCIATESTORE_HPP

#include "basic/AtomicBoolean.hpp"
#include "basic/GenerateRunnable.hpp"
#include "basic/NodeAssociate.hpp"

#include <QObject>


/*

  NodeAssociateStore is used to keep track of and persist the information this
  Node has on its associates. Think of it as a fully automated "contact list".

*/

class NodeAssociateStore: public QObject{
		Q_OBJECT

	private:

		QMap<QString, QSharedPointer<NodeAssociate> > mPeers;
		AtomicBoolean mReady;
		AtomicBoolean mError;
		QString mFilename;

		friend class GenerateRunnable<NodeAssociateStore>;

	public:
		explicit NodeAssociateStore(QString="", QObject *parent=nullptr);
		virtual ~NodeAssociateStore();

	private:
		void bootstrapWorker();
		void load();
		void save();


	public:

		//Make if needed, load if otherwise
		void bootstrap();

		bool isReady(){
			return mReady;
		}
		bool isError(){
			return mError;
		}


		bool hasParticipant(const QString &id);
		QSharedPointer<NodeAssociate> getParticipant(const QString &id);
		QSharedPointer<NodeAssociate> removeParticipant(const QString &id);
		void setParticipant(QSharedPointer<NodeAssociate> participant);


		QMap<QString, QSharedPointer<NodeAssociate> > &getParticipants();

	signals:

		void discoveryStoreReady();
};


#endif // NODEASSOCIATESTORE_HPP

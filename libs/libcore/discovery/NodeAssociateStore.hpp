#ifndef NODEASSOCIATESTORE_HPP
#define NODEASSOCIATESTORE_HPP

#include "basic/AtomicBoolean.hpp"
#include "basic/GenerateRunnable.hpp"
#include "basic/NodeAssociate.hpp"
#include "basic/AsyncStore.hpp"

#include <QObject>


/*

  NodeAssociateStore is used to keep track of and persist the information this
  Node has on its associates. Think of it as a fully automated "contact list".

*/


class NodeAssociateStore: public AsyncStore{
		Q_OBJECT

	private:
		QMap<QString, QSharedPointer<NodeAssociate> > mPeers;
		friend class GenerateRunnable<NodeAssociateStore>;

	public:
		explicit NodeAssociateStore(QString filename="", QObject *parent=nullptr);
		virtual ~NodeAssociateStore();

	protected:
		void bootstrapWorkerImpl() override;
	public:
		void load() override;
		void save() override;


	public:

		bool hasParticipant(const QString &id);
		int getParticipantCount() const ;
		QSharedPointer<NodeAssociate> getParticipant(const QString &id);
		QSharedPointer<NodeAssociate> removeParticipant(const QString &id);
		void setParticipant(QSharedPointer<NodeAssociate> participant);


		QMap<QString, QSharedPointer<NodeAssociate> > &getParticipants();

		void hookSignals(QObject &ob);
		void unHookSignals(QObject &ob);


	signals:

		void peerAdded(QString id);
		void peerRemoved(QString id);
		void peersChanged();
};


#endif // NODEASSOCIATESTORE_HPP


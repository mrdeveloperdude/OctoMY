#ifndef ADDRESSBOOK_HPP
#define ADDRESSBOOK_HPP

#include "basic/AtomicBoolean.hpp"
#include "basic/GenerateRunnable.hpp"
#include "basic/Associate.hpp"
#include "basic/AsyncStore.hpp"

#include <QObject>


/*

  NodeAssociateStore is used to keep track of and persist the information a
  Node has on its associates. Think of it as a fully automated "contact list".

  The store supports loading and saving of the list asynchrounusly.

  NOTE: Currently, for the lack of better alternatives, the Node will store
		details about itself in this store as well.

*/


class AddressBook: public AsyncStore
{
	Q_OBJECT

private:
	QMap<QString, QSharedPointer<Associate> > mAssociates;
	friend class GenerateRunnable<AddressBook>;

public:
	explicit AddressBook(QString filename="", QObject *parent=nullptr);
	virtual ~AddressBook();

protected:
	void bootstrapWorkerImpl() override;
public:
	void load() override;
	void save() override;


public:

	bool hasAssociate(const QString &id);
	int associateCount() const ;
	QSharedPointer<Associate> associateByID(const QString &id);
	QSharedPointer<Associate> removeAssociate(const QString &id);
	void upsertAssociate(QSharedPointer<Associate> associate);


	QMap<QString, QSharedPointer<Associate> > &all();

	void hookSignals(QObject &ob);
	void unHookSignals(QObject &ob);


signals:

	void associateAdded(QString id);
	void associateRemoved(QString id);
	void associatesChanged();
	void addressbookReady(bool);
};


#endif // ADDRESSBOOK_HPP

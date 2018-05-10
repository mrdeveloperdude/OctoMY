#ifndef ADDRESSBOOK_HPP
#define ADDRESSBOOK_HPP

#include "basic/AtomicBoolean.hpp"
#include "basic/GenerateRunnable.hpp"
#include "basic/Associate.hpp"
#include "node/SimpleDataStore.hpp"

#include <QObject>


/*

  AddressBook is used to keep track of and persist the information a
  Node has on its associates. Think of it as a fully automated "contact list".

  The store supports loading and saving of the list asynchrounusly.

*/


class AddressBook: public QObject, public SimpleDataStore
{
	Q_OBJECT

private:
	QMap<QString, QSharedPointer<Associate> > mAssociates;
	friend class GenerateRunnable<AddressBook>;

public:
	explicit AddressBook(QString filename="", QObject *parent=nullptr);
	virtual ~AddressBook();


	// SimpleDataStore interface
public:
	bool fromMap(QVariantMap data) Q_DECL_OVERRIDE;
	QVariantMap toMap() Q_DECL_OVERRIDE;


public:
	bool hasAssociate(const QString &id);
	int associateCount() const ;
	QSharedPointer<Associate> associateByID(const QString &id);
	QSharedPointer<Associate> removeAssociate(const QString &id);
	void upsertAssociate(QSharedPointer<Associate> associate);

	QMap<QString, QSharedPointer<Associate> > &all();

	void setHookSignals(QObject &ob, bool hook);

signals:

	void associateAdded(QString id);
	void associateRemoved(QString id);
	void associatesChanged();


public:

	friend const QDebug &operator<<(QDebug &d, AddressBook &ks);


};



const QDebug &operator<<(QDebug &d, AddressBook &ks);


#endif // ADDRESSBOOK_HPP

#ifndef ADDRESSBOOK_HPP
#define ADDRESSBOOK_HPP

#include "utility/concurrent/AtomicBoolean.hpp"
#include "utility/concurrent/GenerateRunnable.hpp"
#include "address/Associate.hpp"
#include "store/SimpleDataStore.hpp"

#include <QObject>

/** @brief The AddressBook class is used to keep track of and persist the information a
  Node has on its associates. Think of it as a fully automated "contact list".

  The store supports loading and saving of the list asynchrounusly.

 * See also Associate.hpp, KeyStore.hpp, LocalIdentityStore.hpp
*/

class AddressBook : public QObject, public SimpleDataStore
{
	Q_OBJECT
private:
	QMap<QString, QSharedPointer<Associate>> mAssociates;
	ConfigureHelper mConfigureHelper;

private:
	friend class GenerateRunnable<AddressBook>;

public:
	explicit AddressBook(QObject *parent = nullptr);
	virtual ~AddressBook() Q_DECL_OVERRIDE;

	// ConfigureHelper interface
public:
	void configure(QString filename);
	void activate(const bool on, std::function<void(bool)> callBack = nullptr);
	bool isActivated() const;

	// SimpleDataStore interface
public:
	bool fromMap(QVariantMap data) Q_DECL_OVERRIDE;
	QVariantMap toMap() Q_DECL_OVERRIDE;
	bool fromDefault() Q_DECL_OVERRIDE;

public:
	bool hasAssociate(const QString &id);
	int associateCount() const;
	QSharedPointer<Associate> associateByID(const QString &id);
	QSharedPointer<Associate> removeAssociate(const QString &id);
	void upsertAssociate(QSharedPointer<Associate> associate);
	QMap<QString, QSharedPointer<Associate>> all();
	void hookSignals(QObject &ob, bool hook);

signals:
	void associateAdded(QString id);
	void associateRemoved(QString id);
	void associatesChanged();

public:
	friend const QDebug &operator<<(QDebug &d, AddressBook &ks);
};

const QDebug &operator<<(QDebug &d, AddressBook &ks);

#endif
// ADDRESSBOOK_HPP

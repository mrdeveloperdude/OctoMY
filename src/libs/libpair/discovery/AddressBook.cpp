#include "AddressBook.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QFile>
#include <QThreadPool>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QVariantMap>
#include <QVariantList>

AddressBook::AddressBook(QObject *parent) : QObject(parent), mConfigureHelper("AddressBook", true, true, false, true, false)
{
	OC_METHODGATE();
	setObjectName("AddressBook");
}


AddressBook::~AddressBook()
{
	OC_METHODGATE();
}


void AddressBook::configure(QString filename)
{
	OC_METHODGATE();
	if (mConfigureHelper.configure()) {
		SimpleDataStore::configure(filename);
	}
}


void AddressBook::activate(const bool on, std::function<void(bool)> callBack)
{
	OC_METHODGATE();
	if (mConfigureHelper.activate(on)) {
		SimpleDataStore::activate(on, callBack);
	}
}


bool AddressBook::isActivated() const
{
	OC_METHODGATE();
	return mConfigureHelper.isActivated();
}


bool AddressBook::fromMap(QVariantMap map)
{
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		mAssociates.clear();
		QVariantList peers = map["peers"].toList();
		for (QVariantList::iterator b = peers.begin(), e = peers.end(); b != e; ++b) {
			auto peer = QSharedPointer<Associate>::create((*b).toMap());
			upsertAssociate(peer);
		}
		return true;
	}
	return false;
}


QVariantMap AddressBook::toMap()
{
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		QVariantList remotes;
		for (QMap<QString, QSharedPointer<Associate>>::const_iterator b = mAssociates.begin(), e = mAssociates.end(); b != e; ++b) {
			remotes.push_back(b.value()->toVariantMap());
		}
		QVariantMap map;
		map["peers"] = remotes;
		return map;
	}
	return QVariantMap();
}


bool AddressBook::fromDefault()
{
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		mAssociates.clear();
		// synchronize();
		return true;
	}
	return false;
}


bool AddressBook::hasAssociate(const QString &id)
{
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		return (mAssociates.find(id) != mAssociates.end());
	}
	return false;
}


int AddressBook::associateCount() const
{
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		return mAssociates.size();
	}
	return 0;
}


QSharedPointer<Associate> AddressBook::associateByID(const QString &id)
{
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		if (hasAssociate(id)) {
			return mAssociates[id];
		}
	}
	QSharedPointer<Associate> ret;
	return ret;
}


QSharedPointer<Associate> AddressBook::removeAssociate(const QString &id)
{
	OC_METHODGATE();
	QSharedPointer<Associate> ret;
	if (mConfigureHelper.isConfiguredAsExpected()) {
		if (hasAssociate(id)) {
			ret = mAssociates[id];
			mAssociates.remove(id);
			emit associateRemoved(id);
			emit associatesChanged();
			synchronize();
		}
	}
	return ret;
}


void AddressBook::upsertAssociate(QSharedPointer<Associate> associate)
{
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		if (!associate.isNull()) {
			const auto id = associate->id();
			const bool isNew = !hasAssociate(id);
			if(mDebug){
				qDebug().noquote().nospace() << (isNew ? "REGISTERING NEW" : "UPDATING EXISTING") << " ASSOCIATE WITH ID: " << id;
			}
			mAssociates[id] = associate;
			if (isNew) {
				emit associateAdded(id);
			} else {
				// TODO: Should we detect changes here?
			}
			// Allways emit change
			emit associatesChanged();

		} else {
			qWarning() << "ASSOCIATE WAS NULL";
		}
	}
}


QMap<QString, QSharedPointer<Associate>> AddressBook::all()
{
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		return mAssociates;
	}
	return QMap<QString, QSharedPointer<Associate>>();
}


QMap<QString, QSharedPointer<Associate>> AddressBook::filter(QVector<AddressQueryRule> rules)
{
	OC_METHODGATE();
	QMap<QString, QSharedPointer<Associate>> ret;
	if (mConfigureHelper.isConfiguredAsExpected()) {
		
		for (auto it = mAssociates.begin(); it != mAssociates.end(); ++it) {
			auto associate = it.value();
			bool include = false;
			for (const auto &rule : rules) {
				if (associate->type() == rule.type) {
					include = true;
					break;
				}
				/*
				if (rule.trustUs) {
					if (!associate->trusts().hasTrust(TrustList::giveControl)) {
						include = false;
						break;
					}
				}
				if (rule.trustThem) {
					if (!associate->trusts().hasTrust(TrustList::takeControl)) {
						include = false;
						break;
					}
				}
				if (!rule.include) {
					include = false;
					break;
				}
*/
			}
			
			if (include) {
				ret.insert(it.key(), associate);
			}
		}
	}
	return ret;
}


// Forward the async storeReady signal
void AddressBook::hookSignals(QObject &ob, bool hook)
{
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		if (hook) {
			if (!connect(this, SIGNAL(associateAdded(QString)), &ob, SLOT(onAssociateAdded(QString)), OC_CONTYPE)) {
				qWarning() << "Could not connect " << ob.objectName();
			}
			if (!connect(this, SIGNAL(associateRemoved(QString)), &ob, SLOT(onAssociateRemoved(QString)), OC_CONTYPE)) {
				qWarning() << "Could not connect " << ob.objectName();
			}
			if (!connect(this, SIGNAL(associatesChanged()), &ob, SLOT(onAssociateChanged()), OC_CONTYPE)) {
				qWarning() << "Could not connect " << ob.objectName();
			}
		} else {
			if (!disconnect(this, SIGNAL(associateAdded(QString)), &ob, SLOT(onAssociateAdded(QString)))) {
				qWarning() << "Could not disconnect " << ob.objectName();
			}
			if (!disconnect(this, SIGNAL(associateRemoved(QString)), &ob, SLOT(onAssociateRemoved(QString)))) {
				qWarning() << "Could not disconnect " << ob.objectName();
			}
			if (!disconnect(this, SIGNAL(associatesChanged()), &ob, SLOT(onAssociateChanged()))) {
				qWarning() << "Could not disconnect " << ob.objectName();
			}
		}
	}
}


// TODO: Look at converting this to use toStrting() and adding cast operators for QString like is common in many other classes
const QDebug &operator<<(QDebug &d, AddressBook &ab)
{
	OC_FUNCTIONGATE();
	d.nospace() << "AddressBook{ fn=" << ab.filename() << ", fexists=" << ab.fileExists() << ", ready=" << static_cast<const bool>(ab.ready()) << ", peers:[";
	for (QMap<QString, QSharedPointer<Associate>>::iterator b = ab.mAssociates.begin(), e = ab.mAssociates.end(); b != e; ++b) {
		QString key = b.key();
		// b.value();
		d.nospace() << " + " << key;
	}
	d.nospace() << "]}";
	return d.maybeSpace();
}

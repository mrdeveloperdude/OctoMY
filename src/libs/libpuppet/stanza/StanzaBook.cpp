#include "StanzaBook.hpp"

#include "stanza/Stanza.hpp"
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

StanzaBook::StanzaBook()
{
	OC_METHODGATE();
}


StanzaBook::~StanzaBook()
{
	OC_METHODGATE();
}


bool StanzaBook::fromList(const QVariantList &stanzas)
{
	OC_METHODGATE();
	mStanzas.clear();
	for (const auto &item:stanzas) {
		upsertStanza(QSharedPointer<Stanza>::create(item.toMap()));
	}
	return true;
}


QVariantList StanzaBook::toList() const
{
	OC_METHODGATE();
	QVariantList stanzas;
	for (QMap<QString, QSharedPointer<Stanza>>::const_iterator b = mStanzas.begin(), e = mStanzas.end(); b != e; ++b) {
		stanzas.push_back(b.value()->toVariantMap());
	}
	return stanzas;
}


bool StanzaBook::hasStanza(const QString &id)
{
	OC_METHODGATE();
	return (mStanzas.find(id) != mStanzas.end());
}


int StanzaBook::stanzaCount() const
{
	OC_METHODGATE();
	return mStanzas.size();
}


QSharedPointer<Stanza> StanzaBook::stanzaByID(const QString &id)
{
	OC_METHODGATE();
	if (hasStanza(id)) {
		return mStanzas[id];
	}
	return nullptr;
}


QSharedPointer<Stanza> StanzaBook::removeStanza(const QString &id)
{
	OC_METHODGATE();
	QSharedPointer<Stanza> ret;
	if (hasStanza(id)) {
		ret = mStanzas[id];
		mStanzas.remove(id);
	}
	return ret;
}


void StanzaBook::upsertStanza(QSharedPointer<Stanza> stanza)
{
	OC_METHODGATE();
	if (!stanza.isNull()) {
		const auto id = stanza->id();
		const bool isNew = !hasStanza(id);
		if(mDebug){
			qDebug().noquote().nospace() << (isNew ? "REGISTERING NEW" : "UPDATING EXISTING") << " Stanza WITH ID: " << id;
		}
		mStanzas[id] = stanza;
	} else {
		qWarning() << "Stanza WAS NULL";
	}
}


QMap<QString, QSharedPointer<Stanza>> StanzaBook::all()
{
	OC_METHODGATE();
	return mStanzas;
}


QMap<QString, QSharedPointer<Stanza>> StanzaBook::filter(QVector<StanzaQueryRule> )
{
	OC_METHODGATE();
	QMap<QString, QSharedPointer<Stanza>> ret;
		/*
		for (auto it = mStanzas.begin(); it != mStanzas.end(); ++it) {
			auto stanza = it.value();
			bool include = true;
			for (const auto &rule : rules) {
				if (stanza->type() != rule.type) {
					include = false;
					break;
				}
				if (rule.trustUs) {
					if (!stanza->trusts().hasTrust(TrustList::giveControl)) {
						include = false;
						break;
					}
				}
				if (rule.trustThem) {
					if (!stanza->trusts().hasTrust(TrustList::takeControl)) {
						include = false;
						break;
					}
				}
				if (!rule.include) {
					include = false;
					break;
				}
			}
			
			if (include) {
				ret.insert(it.key(), stanza);
			}
		}
*/
	return ret;
}



// TODO: Look at converting this to use toStrting() and adding cast operators for QString like is common in many other classes
const QDebug &operator<<(QDebug &d, StanzaBook &ab)
{
	OC_FUNCTIONGATE();
	d.nospace() << "StanzaBook{ stanzas:[";
	for (QMap<QString, QSharedPointer<Stanza>>::iterator b = ab.mStanzas.begin(), e = ab.mStanzas.end(); b != e; ++b) {
		QString key = b.key();
		// b.value();
		d.nospace() << " + " << key;
	}
	d.nospace() << "]}";
	return d.maybeSpace();
}

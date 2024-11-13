#ifndef STANZABOOK_HPP
#define STANZABOOK_HPP

#include "uptime/SharedPointerWrapper.hpp"

#include <QString>
#include <QMap>

class Stanza;


struct StanzaQueryRule{
	QString type;
	bool trustUs;
	bool trustThem;
	bool include;
	StanzaQueryRule(QString type, bool trustUs=true, bool trustThem=true, bool include=true):
		  type(type)
		, trustUs(trustUs)
		, trustThem(trustThem)
		, include(include)
	{
	}
};


/** @brief The StanzaBook class is used to keep track of the information an Agent
 *  has on its stanzas. Stanzas are physical configurations that the agent supports".
 *  StanzaBook works together with AgentConfig to achive persistence
 * \sa Stanza.hpp AgentConfig AgentConfigStore AgentConfigService
*/

class StanzaBook
{
private:
	QMap<QString, QSharedPointer<Stanza> > mStanzas;
	bool mDebug{false};

public:
	explicit StanzaBook();
	StanzaBook(const StanzaBook &) = default;
	StanzaBook(StanzaBook &&) = delete;
	StanzaBook &operator=(const StanzaBook &) = default;
	StanzaBook &operator=(StanzaBook &&) = delete;
	~StanzaBook();

public:
	bool fromList(const QVariantList &stanzas);
	QVariantList toList() const;

public:
	bool hasStanza(const QString &id);
	int stanzaCount() const;
	QSharedPointer<Stanza> stanzaByID(const QString &id);
	QSharedPointer<Stanza> removeStanza(const QString &id);
	void upsertStanza(QSharedPointer<Stanza> stanza);
	QMap<QString, QSharedPointer<Stanza>> all();
	QMap<QString, QSharedPointer<Stanza>> filter(QVector<StanzaQueryRule> rules);

public:
	friend const QDebug &operator<<(QDebug &d, StanzaBook &ks);

};

const QDebug &operator<<(QDebug &d, StanzaBook &ks);

#endif
// STANZABOOK_HPP

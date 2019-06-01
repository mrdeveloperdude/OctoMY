#ifndef TRUSTLIST_HPP
#define TRUSTLIST_HPP

#include "node/NodeType.hpp"
#include "widgets/pairing/TrustLevel.hpp"

#include <QSet>
#include <QString>


class TrustList:public QSet<QString>
{
private:
	const static QString giveControl;
	const static QString takeControl;
	const static QString block;

public:
	TrustList();
	TrustList(const QSet<QString> &l);
	TrustList(const QStringList &l);

	// Basic trusts management
public:
	void clearTrust();
	void addTrust(QString trust);
	void addTrusts(QSet<QString> trusts);
	void removeTrust(QString trust);
	bool hasTrust(QString trust);

	// Advanced trust management
public:
	void applyTrustLevel(TrustLevel level, NodeType type);
	TrustLevel toTrustLevel(NodeType type);
};

#endif
// TRUSTLIST_HPP

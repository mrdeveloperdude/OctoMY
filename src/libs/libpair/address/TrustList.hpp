#ifndef TRUSTLIST_HPP
#define TRUSTLIST_HPP

#include "node/NodeType.hpp"
#include "pairing/TrustLevel.hpp"

#include <QSet>
#include <QString>
#include <QList>
#include <QStringList>

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
#ifdef Q_COMPILER_INITIALIZER_LISTS
	inline TrustList(std::initializer_list<QString> args) : QSet<QString>(args) { }
#endif

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

	//Helpers
public:
	QStringList toStringList() const;


};

#endif
// TRUSTLIST_HPP

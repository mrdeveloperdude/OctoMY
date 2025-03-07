#ifndef DISCOVERYSERVERSESSION_HPP
#define DISCOVERYSERVERSESSION_HPP



#include "uptime/SharedPointerWrapper.hpp"

#include <QDateTime>
#include <QMap>
#include <QVariantMap>


class DiscoveryServerEntry;

/**
 * @brief The DiscoveryServerSession class represents the list of participants that
 * match in the discovery server. The discovery server will limit communications
 * so that only participants in the same session may exchange data via the server.
 *
 * The participants are expected to provide authentication tokens such as pins
 * and GPS coordinates that allow the discovery server to shard participants into
 * the correct session.
 *
 * The ultimate purpose is scalability and security.
 *  + We don't want to see all the world's nodes in one list when pairing
 *  + We don't want to allow unknown possibly adversarial nodes
 *
 * Beyond the automatic creation of sessions it offers registration and lookup
 * of participants together with a pruning method to get rid of expired entries.
 *
 */
class DiscoveryServerSession
{
private:
	QMap<QString, DiscoveryServerEntry > mParticipantsByID;

public:
	explicit DiscoveryServerSession();

public:
	// Insert paricipant into sesion unless overcrowded. Return success.
	bool set(DiscoveryServerEntry entry);
	// Check if participant is in session
	bool has(QString id);
	// Return full map of this session
	QVariantList toVariantMap();
	// Remove all entries older than provided deadline
	quint64 prune(quint64 deadline);
	//Return number of particpants in this session
	quint64 count();

};

#endif
// DISCOVERYSERVERSESSION_HPP

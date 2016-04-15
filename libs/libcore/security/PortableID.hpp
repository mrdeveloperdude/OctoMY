#ifndef PORTABLEID_HPP
#define PORTABLEID_HPP

#include <QString>

/**
 * @brief The PortableID class provides a mechanism to generate a portable
 * identification string for a tier in the OctoMY™ system.
 *
 *
 */


class PortableID
{

	public:
		QString tier;
		QString name;
		QString id;
		QString pubkey;

	public:
		PortableID();

	public:

		bool fromString(QString);
		QString toString();
};

#endif // PORTABLEID_HPP

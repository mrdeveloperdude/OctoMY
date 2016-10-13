#ifndef PORTABLEID_HPP
#define PORTABLEID_HPP

#include "discovery/DiscoveryRole.hpp"

#include <QString>
#include <QVariantMap>


/**
 * @brief The PortableID class provides a mechanism to generate a portable
 * identification string for a tier in the OctoMY™ system.
 * This is currently used by camera pairing, and will also
 * be used in other pairing
 *
 */



class PortableID
{

	private:
		QString mName;
		QString mGender;
		QString mID;
		quint64 mBirthDate;
		DiscoveryType mType;

	public:
		explicit PortableID();
		explicit PortableID(QVariantMap &data);
		virtual ~PortableID();


	public:

		void setName(QString name);
		void setGender(QString gender);
		void setID(QString id);
		void setBirthDate(quint64 birthDate);
		void setType(DiscoveryType type);

		QString name() const;
		QString gender() const;
		QString id() const;
		quint64 birthDate() const;
		DiscoveryType type() const;

	public:

		bool fromPortableString(QString);
		QString toPortableString();
};

#endif // PORTABLEID_HPP

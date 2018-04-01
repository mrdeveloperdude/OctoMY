#ifndef PORTABLEID_HPP
#define PORTABLEID_HPP

#include "node/NodeRole.hpp"
#include "node/NodeType.hpp"
#include "utility/Standard.hpp"

#include <QString>
#include <QVariantMap>
#include <QDebug>
#include <QRegularExpression>

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
	NodeType mType;

	static const QString SEP;
	static const QString SEP_RE;
	static const QRegularExpression sepRE;
public:
	static const QString dateFMT;

public:
	explicit PortableID();
	explicit PortableID(QVariantMap &data);
	virtual ~PortableID();


public:

	void setName(QString name);
	void setGender(QString gender);
	void setID(QString id);
	void setBirthDate(quint64 birthDate);
	void setType(NodeType type);

	QString name() const;
	QString identifier() const;
	QString gender() const;
	QString id() const;
	quint64 birthDate() const;
	NodeType type() const;

public:

	bool fromPortableString(QString);
	QString toPortableString() const;


};

const QDebug &operator<<(QDebug &d, const PortableID &ass);


#endif // PORTABLEID_HPP

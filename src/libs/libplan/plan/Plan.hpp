#ifndef PLAN_HPP
#define PLAN_HPP

#include <QEnableSharedFromThis>
#include <QPixmap>

/**
 * @brief The Plan class is a means to store an executable manifest that controls
 * one or more nodes in an OctoMY ecosystem.
 * 
 * The manifest is written in plan syntax.
 * 
 * plan will have access to all the features of the node given enough permissions.
 * 
 * plan facilitates collaboration between nodes of different affiliation.

 */

class Plan: public QEnableSharedFromThis<Plan>
{
	
private:
	// The id of this plan
	QString mID;
	// The name of this plan
	QString mName;
	// The content of this plan
	QString mContent;
	// Timestamp of creation of plan
	quint64 mCreatedMS;
	// Timestamp of last change to content
	quint64 mLastUpdatedMS;

public:
	explicit Plan();
	explicit Plan(const QVariantMap map);
	virtual ~Plan();

private:
	void updateTimestamps();

public:
	bool update(const QVariantMap map, bool trustedSource = false);
	
	
public:
	// Return the ID string
	QString id(); //NOTE no const please;
	// Manage content
	void setContent(const QString &content);
	QString content() const;
	// Manage the name
	void setName(const QString &name);
	QString name() const;
	// Provide the create timestamp
	quint64 createdMS() const ;
	// Provide the last updated timestamp
	quint64 lastUpdatedMS() const ;
	// Provide a unique human readible identifier string
	// Unlike name() this always returns something. 
	QString identifier() const;
	// Preview for models
	QPixmap previewPixmap();


public: // Converting selectors to/from different formats
	QVariantMap toVariantMap();
	void fromVariantMap(const QVariantMap map);
	QString toString();
	QMap<QString, QString> toMap();
	
public: // Operators
	bool operator==(Plan &o);
	bool operator!=(Plan &o);

};


const QDebug &operator<<(QDebug &d, Plan &plan);


#endif // PLAN_HPP

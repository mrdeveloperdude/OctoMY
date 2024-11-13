#ifndef SERVICELEVEL_HPP
#define SERVICELEVEL_HPP

#include <QString>
#include <QSet>
#include <QObject>

class ServiceLevel: public QObject
{
	Q_OBJECT
private:
	const QString mName;
	QSet<QString> mServiceNames;
	bool mEnableWanted;

public:
	explicit ServiceLevel(QString name, QSet<QString> serviceNames);
	virtual ~ServiceLevel();

	// Helpers
public:
	bool enabled() const;
	QString name() const;
	QSet<QString> serviceNames() const;
	void toggleService(const QString &name, bool on = true);

public:
	void enable(const bool wanted);

signals:
	void managerChange(const bool wanted);
	void enabled(const bool actual);

};

#endif
// SERVICELEVEL_HPP

#ifndef PLANBOOK_HPP
#define PLANBOOK_HPP

#include "utility/concurrent/GenerateRunnable.hpp"
#include "plan/Plan.hpp"
#include "store/SimpleDataStore.hpp"

#include <QObject>


struct PlanQueryRule{
	QString filter;
	PlanQueryRule(const QString &filter)
		: filter(filter)
	{
	}
};


/** @brief The PlanBook class is used to keep track of and persist the information a
  Node has on its plans. .

  The store supports loading and saving of the list asynchrounusly.

 * See also Plan.hpp
*/

class PlanBook : public QObject, public SimpleDataStore
{
	Q_OBJECT
private:
	QMap<QString, QSharedPointer<Plan> > mPlans;
	QString mDefaultPlan;
	bool mDebug{true};
	ConfigureHelper mConfigureHelper;
	
private:
	friend class GenerateRunnable<PlanBook>;
	
public:
	explicit PlanBook(QObject *parent = nullptr);
	virtual ~PlanBook() override;
	
	// ConfigureHelper interface
public:
	void configure(QString filename);
	void activate(const bool on, std::function<void(bool)> callBack = nullptr);
	bool isActivated() const;
	
	// SimpleDataStore interface
public:
	bool fromMap(QVariantMap map) override;
	QVariantMap toMap() override;
	bool fromDefault() override;
	
public:
	bool hasPlan(const QString &id);
	int planCount() const;
	QSharedPointer<Plan> planByID(const QString &id);
	QString defaultPlan() const;
	QSharedPointer<Plan> removePlan(const QString &id);
	void setDefaultPlan(const QString &id);
	void upsertPlan(QSharedPointer<Plan> plan);
	QMap<QString, QSharedPointer<Plan>> all();
	QMap<QString, QSharedPointer<Plan>> filter(QVector<PlanQueryRule> rules);
	void hookSignals(QObject &ob, bool hook);
	
signals:
	void planAdded(QString id);
	void planRemoved(QString id);
	void plansChanged();
	
public:
	friend const QDebug &operator<<(QDebug &d, PlanBook &ks);
	
};

const QDebug &operator<<(QDebug &d, PlanBook &ks);



#endif // PLANBOOK_HPP

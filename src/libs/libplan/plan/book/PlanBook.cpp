#include "PlanBook.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QFile>
#include <QThreadPool>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QVariantMap>
#include <QVariantList>

PlanBook::PlanBook(QObject *parent)
	: QObject(parent)
	, SimpleDataStore(false)
	, mConfigureHelper("PlanBook", true, true, false, true, false)
{
	OC_METHODGATE();
	setObjectName("PlanBook");
	SimpleDataStore::setDebug(true);
}


PlanBook::~PlanBook()
{
	OC_METHODGATE();
}


void PlanBook::configure(QString filename)
{
	OC_METHODGATE();
	if (mConfigureHelper.configure()) {
		SimpleDataStore::configure(filename);
	}
}


void PlanBook::activate(const bool on, std::function<void(bool)> callBack)
{
	OC_METHODGATE();
	if (mConfigureHelper.activate(on)) {
		SimpleDataStore::activate(on, callBack);
	}
}


bool PlanBook::isActivated() const
{
	OC_METHODGATE();
	return mConfigureHelper.isActivated();
}


bool PlanBook::fromMap(QVariantMap map)
{
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		mPlans.clear();
		qDebug()<<"FROM MAP GOT" << map;
		mDefaultPlan = map["default"].toString();
		auto plans = map["plans"].toList();
		for (auto &p:plans) {
			auto plan = QSharedPointer<Plan>::create(p.toMap());
			upsertPlan(plan);
		}
		return true;
	}
	return false;
}


QVariantMap PlanBook::toMap()
{
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		QVariantList out;
		for (auto &plan:mPlans.values()) {
			out.push_back(plan->toVariantMap());
		}
		QVariantMap map;
		map["plans"] = out;
		map["default"] = mDefaultPlan;
		qDebug()<<"TO MAP MADE" << map;
		return map;
	}
	return QVariantMap();
}


bool PlanBook::fromDefault()
{
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		mPlans.clear();
		mDefaultPlan = QString();
		// synchronize();
		return true;
	}
	return false;
}


bool PlanBook::hasPlan(const QString &id)
{
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		return (mPlans.find(id) != mPlans.end());
	}
	return false;
}


int PlanBook::planCount() const
{
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		return mPlans.size();
	}
	return 0;
}


QSharedPointer<Plan> PlanBook::planByID(const QString &id)
{
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		if (hasPlan(id)) {
			return mPlans[id];
		}
	}
	QSharedPointer<Plan> ret;
	return ret;
}

QString PlanBook::defaultPlan() const{
	OC_METHODGATE();
	QString ret;
	if (mConfigureHelper.isConfiguredAsExpected()) {
		ret = mDefaultPlan;
	}
	return ret;
	
}

QSharedPointer<Plan> PlanBook::removePlan(const QString &id)
{
	OC_METHODGATE();
	QSharedPointer<Plan> ret;
	if (mConfigureHelper.isConfiguredAsExpected()) {
		if (hasPlan(id)) {
			ret = mPlans[id];
			if(ret){
				if(ret->id() == mDefaultPlan){
					mDefaultPlan = "";
				}
			}
			mPlans.remove(id);
			emit planRemoved(id);
			emit plansChanged();
			save();
		}
		else{
			qWarning() << "Trying to remove non-existant plan, skipping...";
		}
	}
	return ret;
}

void PlanBook::setDefaultPlan(const QString &id){
	OC_METHODGATE();
	if(hasPlan(id)){
		mDefaultPlan = id;
		save();
	}
	else{
		qWarning()<<"Trying to set non-existant default plan, skipping...";
	}
}

void PlanBook::upsertPlan(QSharedPointer<Plan> plan)
{
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		if (!plan.isNull()) {
			const auto id = plan->id();
			const bool isNew = !hasPlan(id);
			if(mDebug){
				qDebug().noquote().nospace() << (isNew ? "REGISTERING NEW" : "UPDATING EXISTING") << " PLAN WITH ID: " << id;
			}
			mPlans[id] = plan;
			save();
			if (isNew) {
				emit planAdded(id);
			} else {
				// TODO: Should we detect changes here?
			}
			// Always emit change
			emit plansChanged();
		} else {
			qWarning() << "Trying to upsert null plan, skipping...";
		}
	}
}


QMap<QString, QSharedPointer<Plan>> PlanBook::all()
{
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		return mPlans;
	}
	return QMap<QString, QSharedPointer<Plan>>();
}


QMap<QString, QSharedPointer<Plan>> PlanBook::filter(QVector<PlanQueryRule> rules)
{
	OC_METHODGATE();
	QMap<QString, QSharedPointer<Plan>> ret;
	if (mConfigureHelper.isConfiguredAsExpected()) {
		
		for (auto it = mPlans.begin(); it != mPlans.end(); ++it) {
			auto plan = it.value();
			auto planName = plan->name().toLower().trimmed();
			bool include = false;
			for (const auto &rule : rules) {
				auto ruleFilter = rule.filter.toLower().trimmed();
				if (ruleFilter.isEmpty() || planName.contains(ruleFilter)) {
					include = true;
					break;
				}
			}
			if (include) {
				ret.insert(it.key(), plan);
			}
		}
	}
	return ret;
}


// Forward the async storeReady signal
void PlanBook::hookSignals(QObject &ob, bool hook)
{
	OC_METHODGATE();
	if (mConfigureHelper.isConfiguredAsExpected()) {
		if (hook) {
			if (!connect(this, SIGNAL(planAdded(QString)), &ob, SLOT(onPlanAdded(QString)), OC_CONTYPE)) {
				qWarning() << "Could not connect " << ob.objectName();
			}
			if (!connect(this, SIGNAL(planRemoved(QString)), &ob, SLOT(onPlanRemoved(QString)), OC_CONTYPE)) {
				qWarning() << "Could not connect " << ob.objectName();
			}
			if (!connect(this, SIGNAL(plansChanged()), &ob, SLOT(onPlanChanged()), OC_CONTYPE)) {
				qWarning() << "Could not connect " << ob.objectName();
			}
		} else {
			if (!disconnect(this, SIGNAL(planAdded(QString)), &ob, SLOT(onPlanAdded(QString)))) {
				qWarning() << "Could not disconnect " << ob.objectName();
			}
			if (!disconnect(this, SIGNAL(planRemoved(QString)), &ob, SLOT(onPlanRemoved(QString)))) {
				qWarning() << "Could not disconnect " << ob.objectName();
			}
			if (!disconnect(this, SIGNAL(plansChanged()), &ob, SLOT(onPlanChanged()))) {
				qWarning() << "Could not disconnect " << ob.objectName();
			}
		}
	}
}


// TODO: Look at converting this to use toStrting() and adding cast operators for QString like is common in many other classes
const QDebug &operator<<(QDebug &d, PlanBook &ab)
{
	OC_FUNCTIONGATE();
	d.nospace() << "PlanBook{ fn=" << ab.filename() << ", fexists=" << ab.fileExists() << ", ready=" << static_cast<const bool>(ab.ready()) << ", plans:[";
	for (QMap<QString, QSharedPointer<Plan>>::iterator b = ab.mPlans.begin(), e = ab.mPlans.end(); b != e; ++b) {
		QString key = b.key();
		// b.value();
		d.nospace() << " + " << key;
	}
	d.nospace() << "]}";
	return d.maybeSpace();
}

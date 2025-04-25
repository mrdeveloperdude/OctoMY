#include "PlanBookModel.hpp"


#include "plan/Plan.hpp"
#include "plan/book/PlanBook.hpp"
#include "uptime/ConnectionType.hpp"
#include "uptime/MethodGate.hpp"
#include "utility/string/String.hpp"


#include <QDebug>
#include <QList>
#include <qicon.h>

PlanBookModel::PlanBookModel(const QSharedPointer<PlanBook> &store, QObject* parent)
	: QAbstractTableModel(parent)
	, mStore(store)
{
	OC_METHODGATE();
	if(!mStore.isNull()) {
		if(!connect(mStore.data(), &PlanBook::plansChanged, this, &PlanBookModel::onPlansChanged, OC_CONTYPE)) {
			//if(!connect(&mStore, SIGNAL(peersChanged()), this, SLOT(onPlanBookChanged()), OC_CONTYPE)) {
			qWarning()<<"ERROR: Could not connect ";
		}
	} else {
		qWarning()<<"ERROR: No store";
	}
}


PlanBookModel::~PlanBookModel()
{
	OC_METHODGATE();
}


int PlanBookModel::rowCount(const QModelIndex &) const
{
	OC_METHODGATE();
	auto ret{0};
	if(!mStore.isNull()) {
		auto plans{mStore->all()};
		for(auto &p:plans) {
			if(nullptr != p) {
				/* TODO: Implement filter stuff
				auto t = p->type();
				if(filter(t)) {
					ret++;
				}*/
				ret++;
			}
		}
	} else {
		qWarning()<<"ERROR: No store";
	}
	//qDebug() << "PlanBookModel Rowcount: " << ret;
	return ret;
}


int PlanBookModel::columnCount(const QModelIndex &) const
{
	OC_METHODGATE();
	return 4;
}


QVariant PlanBookModel::data(const QModelIndex &index, int role) const
{
	OC_METHODGATE();
	auto filtered = filteredPlans();
	if (!index.isValid() || index.row() < 0 || index.row() >= filtered.size()){
		return QVariant();
	}
	auto plan = filtered.at(index.row());
	
	if (role == Qt::DecorationRole && index.column() == 0) {
		auto pix = plan->previewPixmap();
		if (!pix.isNull()){
			return QIcon(pix);
		}
		return QIcon(":/icons/controller/unknown.svg");
	}
	
	if (role == Qt::DisplayRole) {
		switch (index.column()) {
		case 0:
			return plan->identifier();
		case 1: {
			return utility::string::formattedDateFromMS(plan->createdMS());
		}
		case 2: {
			return utility::string::formattedDateFromMS(plan->lastUpdatedMS());
		}
		case 3: {
			return utility::string::humanReadableSize(plan->content().size());
		}
		default:
			return QVariant();
		}
	}
	return QVariant();
}


QVariant PlanBookModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	OC_METHODGATE();
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		switch (section) {
		case 0: return QString("Plan");
		case 1: return QString("Created");
		case 2: return QString("Last Updated");
		case 3: return QString("Bytesize");
		default: return QVariant();
		}
	}
	return QAbstractTableModel::headerData(section, orientation, role);
}


bool PlanBookModel::filter(const QString &f) const
{
	OC_METHODGATE();
	Q_UNUSED(f);
	return true;
}


QString PlanBookModel::status()
{
	OC_METHODGATE();
	QString out("STATUS: ");
	if(!mStore.isNull()) {
		out += QString::number(mStore->planCount());
	} else {
		out += "no";
	}
	out += " plans";
	return out;
}


QSharedPointer<PlanBook> PlanBookModel::planBook() const{
	OC_METHODGATE();
	return mStore;
}


QSharedPointer<Plan> PlanBookModel::plan(const QModelIndex &index) const{
	OC_METHODGATE();
	auto filtered = filteredPlans();
	if (!index.isValid() || index.row() < 0 || index.row() >= filtered.size()){
		return nullptr;
	}
	return filtered.at(index.row());
}


QList<QSharedPointer<Plan> > PlanBookModel::filteredPlans() const
{
	OC_METHODGATE();
	auto allPlans = mStore->all();
	QList<QSharedPointer<Plan> > filtered;
	for (const auto &plan : std::as_const(allPlans)) {
		bool include = false;
		// TODO: Implement filter
		include = true;
		if (include){
			filtered.append(plan);
		}
	}
	return filtered;
}




void PlanBookModel::onPlansChanged()
{
	// qDebug()<<"FORWARDING UPDATE SIGNAL";
	if(!mStore.isNull()) {
		beginResetModel();
		endResetModel();
		qWarning()<<"Updated data in model for PlanBook";
		emit dataChanged(index(0, 0), index(mStore->planCount(), 0));
	} else {
		qWarning()<<"ERROR: No store";
	}
}



void PlanBookModel::deleteSelected(const QModelIndexList &selectedIndexes)
{
	OC_METHODGATE();
	if(mStore.isNull()){
		qWarning() << "ERROR: No store";
		return;
	}
	// Copy and sort the indexes in descending order by row to avoid any potential shifting issues.
	QModelIndexList indexes = selectedIndexes;
	std::sort(indexes.begin(), indexes.end(), [](const QModelIndex &a, const QModelIndex &b) {
		return a.row() > b.row();
	});
	
	for(const QModelIndex &index : indexes){
		// Retrieve the plan identifier, assuming it is returned using Qt::ToolTipRole
		QVariant idVar = data(index, Qt::ToolTipRole);
		if(idVar.isValid() && idVar.canConvert<QString>()){
			QString id = idVar.toString();
			auto removedPlan = mStore->removePlan(id);
			if(removedPlan.isNull()){
				qWarning() << "Failed to remove plan with id" << id;
			}
		}
	}
	// The store should emit plansChanged(), which will trigger onPlansChanged() and update the view.
}



void PlanBookModel::duplicateSelected(const QModelIndexList &selectedIndexes)
{
	OC_METHODGATE();
	if(mStore.isNull()){
		qWarning() << "ERROR: No store";
		return;
	}
	// Copy and sort the indexes in descending order by row to avoid any potential shifting issues.
	QModelIndexList indexes = selectedIndexes;
	std::sort(indexes.begin(), indexes.end(), [](const QModelIndex &a, const QModelIndex &b) {
		return a.row() > b.row();
	});
	
	for(const QModelIndex &index : indexes){
		// Retrieve the plan identifier, assuming it is returned using Qt::ToolTipRole
		QVariant idVar = data(index, Qt::ToolTipRole);
		if(idVar.isValid() && idVar.canConvert<QString>()){
			QString id = idVar.toString();
			auto removedPlan = mStore->removePlan(id);
			if(removedPlan.isNull()){
				qWarning() << "Failed to remove plan with id" << id;
			}
		}
	}
	// The store should emit plansChanged(), which will trigger onPlansChanged() and update the view.
}

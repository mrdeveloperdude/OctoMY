#ifndef PLANBOOKMODEL_HPP
#define PLANBOOKMODEL_HPP


#include <QDebug>
#include <QAbstractTableModel>

class PlanBook;
class Plan;

class PlanBookModel: public QAbstractTableModel
{
	//Q_OBJECT
private:
	QSharedPointer<PlanBook> mStore;
	QString mFilter;
	
public:
	// Columns:
	// 0: Name
	// 1: Created timestamp
	// 2: Last Updated timestamp
	// 3: Bytesize
	explicit PlanBookModel(const QSharedPointer<PlanBook> &planBook, QObject* parent = nullptr);
	virtual ~PlanBookModel();
	
	// QAbstractTableModel interface
public:
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	
public:
	bool filter(const QString &f) const;
	QString status();
	QSharedPointer<PlanBook> planBook() const;
	QSharedPointer<Plan> plan(const QModelIndex &index) const;
	
private:
	QList<QSharedPointer<Plan> > filteredPlans() const;

public slots:
	void onPlansChanged();
	void deleteSelected(const QModelIndexList &selectedIndexes);
	void duplicateSelected(const QModelIndexList &selectedIndexes);

};


#endif // PLANBOOKMODEL_HPP

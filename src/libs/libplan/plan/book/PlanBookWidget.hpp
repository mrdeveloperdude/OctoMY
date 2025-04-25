#ifndef PLANBOOKWIDGET_HPP
#define PLANBOOKWIDGET_HPP

#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QWidget>

class Node;
class Plan;
class PlanBookModel;

namespace Ui {
class PlanBookWidget;
}

class PlanBookWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::PlanBookWidget *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;
	PlanBookModel *mPlanBookModel{nullptr};
	
	
public:
	explicit PlanBookWidget(QWidget *parent = nullptr);
	~PlanBookWidget();
	
public:
	void configure(QSharedPointer<Node> n);
	
private slots:
	
	void createPlan();
	void deletePlan();
	void duplicatePlan();
	void synchronizePlans();
	void setDefaultPlan();
	
	void itemDoubleClicked(const QModelIndex &index);
	
signals:
	void planSelected(QSharedPointer<Plan> plan);
};

#endif // PLANBOOKWIDGET_HPP

#ifndef PLANBOOKACTIVITY_HPP
#define PLANBOOKACTIVITY_HPP


#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Node;
class Plan;

namespace Ui {
class PlanBookActivity;
}


class PlanBookActivity : public Activity
{
	Q_OBJECT
	
private:
	Ui::PlanBookActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;
	
public:
	explicit PlanBookActivity(QWidget *parent = nullptr);
	~PlanBookActivity();
	
public:
	void configure(QSharedPointer<Node> n);
	
	
	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
	
private slots:
	void planSelected(QSharedPointer<Plan> plan);
};


#endif // PLANBOOKACTIVITY_HPP

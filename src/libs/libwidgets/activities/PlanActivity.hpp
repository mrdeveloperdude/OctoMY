#ifndef PLANACTIVITY_HPP
#define PLANACTIVITY_HPP

#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Node;

namespace Ui {
class PlanActivity;
}


class PlanActivity : public Activity
{
	Q_OBJECT

private:
	Ui::PlanActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit PlanActivity(QWidget *parent = nullptr);
	~PlanActivity();
	
public:
	void configure(QSharedPointer<Node> n);
	

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // PLANACTIVITY_HPP

#ifndef COMPUTEACTIVITY_HPP
#define COMPUTEACTIVITY_HPP

#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Node;

namespace Ui {
class ComputeActivity;
}


class ComputeActivity : public Activity
{
	Q_OBJECT

private:
	Ui::ComputeActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit ComputeActivity(QWidget *parent = nullptr);
	~ComputeActivity();
	
public:
	void configure(QSharedPointer<Node> n);
	

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // COMPUTEACTIVITY_HPP

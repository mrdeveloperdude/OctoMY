#ifndef NODESMANAGEMENTACTIVITY_HPP
#define NODESMANAGEMENTACTIVITY_HPP

#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Node;

namespace Ui {
class NodesManagementActivity;
}


class NodesManagementActivity : public Activity
{
	Q_OBJECT

private:
	Ui::NodesManagementActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit NodesManagementActivity(QWidget *parent = nullptr);
	~NodesManagementActivity();
	
public:
	void configure(QSharedPointer<Node> n);
	

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // NODESMANAGEMENTACTIVITY_HPP

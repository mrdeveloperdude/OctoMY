#ifndef PLANEDITORACTIVITY_HPP
#define PLANEDITORACTIVITY_HPP

#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Node;

namespace Ui {
class PlanEditorActivity;
}


class PlanEditorActivity : public Activity
{
	Q_OBJECT

private:
	Ui::PlanEditorActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit PlanEditorActivity(QWidget *parent = nullptr);
	~PlanEditorActivity();
	
public:
	void configure(QSharedPointer<Node> n);
	

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // PLANEDITORACTIVITY_HPP

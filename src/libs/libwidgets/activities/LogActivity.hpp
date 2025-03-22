#ifndef LOGACTIVITY_HPP
#define LOGACTIVITY_HPP

#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Node;

namespace Ui {
class LogActivity;
}


class LogActivity : public Activity
{
	Q_OBJECT

private:
	Ui::LogActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit LogActivity(QWidget *parent = nullptr);
	~LogActivity();
	
public:
	void configure(QSharedPointer<Node> n);
	

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // LOGACTIVITY_HPP

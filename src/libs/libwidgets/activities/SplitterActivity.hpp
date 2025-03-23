#ifndef SPLITTERACTIVITY_HPP
#define SPLITTERACTIVITY_HPP

#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Node;

namespace Ui {
class SplitterActivity;
}


class SplitterActivity : public Activity
{
	Q_OBJECT

private:
	Ui::SplitterActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit SplitterActivity(QWidget *parent = nullptr);
	~SplitterActivity();
	
public:
	void configure(QSharedPointer<Node> n);
	

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // SPLITTERACTIVITY_HPP

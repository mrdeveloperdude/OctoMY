#ifndef CONSTRUCTACTIVITY_HPP
#define CONSTRUCTACTIVITY_HPP

#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Node;

namespace Ui {
class ConstructActivity;
}


class ConstructActivity : public Activity
{
	Q_OBJECT

private:
	Ui::ConstructActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit ConstructActivity(QWidget *parent = nullptr);
	~ConstructActivity();
	
public:
	void configure(QSharedPointer<Node> n);
	

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // CONSTRUCTACTIVITY_HPP

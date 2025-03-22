#ifndef ACTUATORACTIVITY_HPP
#define ACTUATORACTIVITY_HPP

#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Node;

namespace Ui {
class ActuatorActivity;
}


class ActuatorActivity : public Activity
{
	Q_OBJECT

private:
	Ui::ActuatorActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit ActuatorActivity(QWidget *parent = nullptr);
	~ActuatorActivity();
	
public:
	void configure(QSharedPointer<Node> n);
	

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // ACTUATORACTIVITY_HPP

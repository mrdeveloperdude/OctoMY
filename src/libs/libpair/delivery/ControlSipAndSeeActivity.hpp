#ifndef CONTROLSIPANDSEEACTIVITY_HPP
#define CONTROLSIPANDSEEACTIVITY_HPP

#include <QWidget>
#include "components/navigation/Activity.hpp"

class Node;

namespace Ui {
class ControlSipAndSeeActivity;
}

class ControlSipAndSeeActivity : public Activity
{
	Q_OBJECT
private:
	Ui::ControlSipAndSeeActivity *ui;
	QSharedPointer<Node> mNode;
	
	ConfigureHelper mConfigureHelper;

public:
	explicit ControlSipAndSeeActivity(QWidget *parent = nullptr);
	~ControlSipAndSeeActivity();
	
public:
	void configure(QSharedPointer<Node> mNode);
	void reset();

private slots:
	void done();

	// Virtual activity mechanism
protected:
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
	void pushImpl(const QStringList arguments = QStringList()) override;
};

#endif // CONTROLSIPANDSEEACTIVITY_HPP

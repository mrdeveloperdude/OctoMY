#ifndef AGENTBAPTISMACTIVITY_HPP
#define AGENTBAPTISMACTIVITY_HPP

#include "components/navigation/Activity.hpp"
#include "name/AgentNameGenerator.hpp"
#include "name/GenderGenerator.hpp"

#include <QWidget>

class Node;

namespace Ui {
class AgentBaptismActivity;
}

class AgentBaptismActivity : public Activity
{
	Q_OBJECT
private:
	Ui::AgentBaptismActivity *ui;
	AgentNameGenerator mNameGenerator;
	GenderGenerator mGenderGenerator;
	QSharedPointer<Node> mNode;
	
	ConfigureHelper mConfigureHelper;

public:
	explicit AgentBaptismActivity(QWidget *parent = nullptr);
	~AgentBaptismActivity();
	
private:
	void configureValidators();
	QString name();
	QString gender();
	
public:
	void configure(QSharedPointer<Node> mNode);
	void reset();

private slots:
	void randomizeName();
	void randomizeGender();
	void done();

	// Virtual activity mechanism
protected:
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
	void pushImpl(const QStringList arguments = QStringList()) override;

};

#endif // AGENTBAPTISMACTIVITY_HPP

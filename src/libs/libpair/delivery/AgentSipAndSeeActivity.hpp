#ifndef AGENTSIPANDSEEACTIVITY_HPP
#define AGENTSIPANDSEEACTIVITY_HPP

#include "components/navigation/Activity.hpp"


#include <QWidget>

class Node;
class PortableID;

namespace Ui {
class AgentSipAndSeeActivity;
}

class AgentSipAndSeeActivity : public Activity
{
	Q_OBJECT
private:
	Ui::AgentSipAndSeeActivity *ui;
	QSharedPointer<Node> mNode;
	bool mUseVoice{false};
	
	ConfigureHelper mConfigureHelper;

public:
	explicit AgentSipAndSeeActivity(QWidget *parent = nullptr);
	~AgentSipAndSeeActivity();

private:
	void announceBirth(const PortableID &id);

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

#endif // AGENTSIPANDSEEACTIVITY_HPP

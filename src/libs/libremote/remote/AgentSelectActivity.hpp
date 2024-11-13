#ifndef AGENTSELECTACTIVITY_HPP
#define AGENTSELECTACTIVITY_HPP


#include "uptime/ConfigureHelper.hpp"
#include "components/navigation/Activity.hpp"


class Remote;
class AgentListModel;
class AgentItemDelegate;

namespace Ui {
class AgentSelectActivity;
}

class AgentSelectActivity : public Activity
{
	Q_OBJECT
private:
	Ui::AgentSelectActivity *ui;
	QSharedPointer<Remote> mRemote;
	AgentListModel *mList{nullptr};
	AgentItemDelegate *mDelegate{nullptr};
	bool mDebug{false};
	
	ConfigureHelper mConfigureHelper;

	
public:
	explicit AgentSelectActivity(QWidget *parent = nullptr);
	~AgentSelectActivity();

public:
	void configure(QSharedPointer<Remote> remote);
	void updateList();

	// Virtual activity mechanism
protected:
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
	void pushImpl(const QStringList arguments = QStringList()) override;
	
private slots:
	void startPairing();
	void control();

};

#endif // AGENTSELECTACTIVITY_HPP


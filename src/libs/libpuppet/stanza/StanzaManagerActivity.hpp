#ifndef STANZAMANAGERACTIVITY_HPP
#define STANZAMANAGERACTIVITY_HPP

#include "components/navigation/Activity.hpp"
#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QWidget>

class Agent;
class StanzaListModel;
class StanzaItemDelegate;

namespace Ui {
class StanzaManagerActivity;
}

class StanzaManagerActivity : public Activity
{
	Q_OBJECT
private:
	Ui::StanzaManagerActivity *ui;
	QSharedPointer<Agent> mAgent;
	StanzaListModel *mList;
	StanzaItemDelegate *mDelegate;
	ConfigureHelper mConfigureHelper;
	
	
public:
	explicit StanzaManagerActivity(QWidget *parent = nullptr);
	~StanzaManagerActivity();

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;

public:
	void configure(QSharedPointer<Agent> agent);
	void updateList();

public slots:
	void create();
	void done();

signals:
	void startEdit(const QString id);


};

#endif // STANZAMANAGERACTIVITY_HPP

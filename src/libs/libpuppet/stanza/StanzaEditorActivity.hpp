#ifndef STANZAEDITORACTIVITY_H
#define STANZAEDITORACTIVITY_H

#include <QWidget>
#include "components/navigation/Activity.hpp"


class Agent;
class Stanza;

namespace Ui {
class StanzaEditorActivity;
}

class StanzaEditorActivity : public Activity
{
	Q_OBJECT
private:
	Ui::StanzaEditorActivity *ui;
	QSharedPointer<Agent> mAgent;
	QSharedPointer<Stanza> mStanza;
	

public:
	explicit StanzaEditorActivity(QWidget *parent = nullptr);
	~StanzaEditorActivity();
public:
	void configure(QSharedPointer<Agent> agent);

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;

private slots:
	void selectLobe();
	void save();
	void cancel();
};

#endif // STANZAEDITORACTIVITY_H

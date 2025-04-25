#ifndef MESSAGEACTIVITY_H
#define MESSAGEACTIVITY_H

#include <QWidget>
#include "components/navigation/Activity.hpp"

namespace Ui {
class MessageActivity;
}


class MessageActivity : public Activity
{
	Q_OBJECT

private:
	Ui::MessageActivity *ui;
	QString mID;

public:
	explicit MessageActivity(QWidget *parent = nullptr);
	~MessageActivity();
	
	void end(bool status);
	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
	
private slots:
	void confirmed();
	void canceled();
};

#endif // MESSAGEACTIVITY_H

#ifndef TRIGGERMANAGERACTIVITY_HPP
#define TRIGGERMANAGERACTIVITY_HPP

#include <QWidget>
#include "components/navigation/Activity.hpp"

namespace Ui {
class TriggerManagerActivity;
}


class TriggerManagerActivity : public Activity
{
	Q_OBJECT

private:
	Ui::TriggerManagerActivity *ui;
	QString mID;

public:
	explicit TriggerManagerActivity(QWidget *parent = nullptr);
	~TriggerManagerActivity();
	
public:
	void configure();
	

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // TRIGGERMANAGERACTIVITY_HPP

#ifndef TRANSITIONACTIVITY_H
#define TRANSITIONACTIVITY_H

#include <QWidget>

#include "components/navigation/Activity.hpp"

namespace Ui {
class TransitionActivity;
}

class TransitionActivity : public Activity
{
	Q_OBJECT
	
private:
	Ui::TransitionActivity *ui;

public:
	explicit TransitionActivity(QWidget *parent = nullptr);
	~TransitionActivity();

	// Activity mechanism
protected:
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList());
	void pushImpl(const QStringList arguments = QStringList());

};

#endif // TRANSITIONACTIVITY_H

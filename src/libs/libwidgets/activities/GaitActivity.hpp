#ifndef GAITACTIVITY_HPP
#define GAITACTIVITY_HPP

#include "components/navigation/Activity.hpp"
#include "gait/GaitController.hpp"

#include <QWidget>

namespace Ui {
class GaitActivity;
}


class GaitActivity : public Activity
{
	Q_OBJECT

private:
	Ui::GaitActivity *ui;
	GaitController<qreal> *mGait{nullptr};

public:
	explicit GaitActivity(QWidget *parent = nullptr);
	~GaitActivity();
	
public:
	void configure();
	

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // GAITACTIVITY_HPP

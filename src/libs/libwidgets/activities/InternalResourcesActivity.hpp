#ifndef INTERNALRESOURCESACTIVITY_HPP
#define INTERNALRESOURCESACTIVITY_HPP

#include "components/navigation/Activity.hpp"


namespace Ui {
class InternalResourcesActivity;
}


class InternalResourcesActivity : public Activity
{
	Q_OBJECT

private:
	Ui::InternalResourcesActivity *ui;

public:
	explicit InternalResourcesActivity(QWidget *parent = nullptr);
	~InternalResourcesActivity();
	
public:
	void configure();

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // INTERNALRESOURCESACTIVITY_HPP

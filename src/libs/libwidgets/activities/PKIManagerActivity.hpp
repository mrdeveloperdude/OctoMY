#ifndef PKIMANAGERACTIVITY_HPP
#define PKIMANAGERACTIVITY_HPP

#include <QWidget>
#include "components/navigation/Activity.hpp"

namespace Ui {
class PKIManagerActivity;
}


class PKIManagerActivity : public Activity
{
	Q_OBJECT

private:
	Ui::PKIManagerActivity *ui;

public:
	explicit PKIManagerActivity(QWidget *parent = nullptr);
	~PKIManagerActivity();

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // PKIMANAGERACTIVITY_HPP

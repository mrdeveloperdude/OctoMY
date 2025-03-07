#ifndef IDENTITY_ACTIVITY_HPP
#define IDENTITY_ACTIVITY_HPP

#include "uptime/ConfigureHelper.hpp"
#include "components/navigation/Activity.hpp"

#include <QWidget>
#include <QHostAddress>

namespace Ui
{
class IdentityMenuActivity;
}

class Node;


/**
 * @brief The IdentityMenuActivity class is a menu for the user to select between 
 * available identity management screens
 */

class IdentityMenuActivity : public Activity
{
	Q_OBJECT

private:
	Ui::IdentityMenuActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;


public:
	explicit IdentityMenuActivity(QWidget *parent = nullptr);
	virtual ~IdentityMenuActivity() override;

public:
	void configure(QSharedPointer<Node> node);
	
	// Virtual activity mechanism
protected:
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
	void pushImpl(const QStringList arguments = QStringList()) override;

private slots:
	void birthCertificate();
	void abortion();
	void delivery();
	void done();
};

#endif
// IDENTITY_ACTIVITY_HPP

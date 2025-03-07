#ifndef IDENTITYACTIVITY_HPP
#define IDENTITYACTIVITY_HPP

#include "uptime/ConfigureHelper.hpp"
#include "components/navigation/Activity.hpp"


#include <QWidget>

namespace Ui {
class IdentityActivity;
}

class Node;


/**
 * @brief The IdentityActivity class is a screen that shows all the details
 * pertaingin to this node's identity such as ID, QR code, identicon and more.
 */

class IdentityActivity : public Activity
{
	Q_OBJECT
private:
	Ui::IdentityActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit IdentityActivity(QWidget *parent = nullptr);
	~IdentityActivity();

public:
	void configure(QSharedPointer<Node> node);

	// Virtual activity mechanism
protected:
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
	void pushImpl(const QStringList arguments = QStringList()) override;

};

#endif // IDENTITYACTIVITY_HPP

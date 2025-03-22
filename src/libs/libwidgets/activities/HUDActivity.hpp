#ifndef HUDACTIVITY_HPP
#define HUDACTIVITY_HPP

#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Node;

namespace Ui {
class HUDActivity;
}


class HUDActivity : public Activity
{
	Q_OBJECT

private:
	Ui::HUDActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit HUDActivity(QWidget *parent = nullptr);
	~HUDActivity();
	
public:
	void configure(QSharedPointer<Node> n);
	

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // HUDACTIVITY_HPP

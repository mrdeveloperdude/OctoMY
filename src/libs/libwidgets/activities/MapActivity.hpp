#ifndef MAPACTIVITY_HPP
#define MAPACTIVITY_HPP

#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Node;

namespace Ui {
class MapActivity;
}


class MapActivity : public Activity
{
	Q_OBJECT

private:
	Ui::MapActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit MapActivity(QWidget *parent = nullptr);
	~MapActivity();
	
public:
	void configure(QSharedPointer<Node> n);
	

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // MAPACTIVITY_HPP

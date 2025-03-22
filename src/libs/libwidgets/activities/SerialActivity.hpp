#ifndef SERIALACTIVITY_HPP
#define SERIALACTIVITY_HPP

#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Node;

namespace Ui {
class SerialActivity;
}


class SerialActivity : public Activity
{
	Q_OBJECT

private:
	Ui::SerialActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit SerialActivity(QWidget *parent = nullptr);
	~SerialActivity();
	
public:
	void configure(QSharedPointer<Node> n);
	

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
	
public slots:
	
	void onSerialSettingsChanged();
};

#endif // SERIALACTIVITY_HPP

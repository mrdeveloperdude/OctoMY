#ifndef MEDIADEVICEACTIVITY_H
#define MEDIADEVICEACTIVITY_H

#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Node;

namespace Ui {
class MediaDeviceActivity;
}


class MediaDeviceActivity : public Activity
{
	Q_OBJECT
private:
	Ui::MediaDeviceActivity *ui;
	QSharedPointer<Node> mNode;

public:
	explicit MediaDeviceActivity(QWidget *parent = nullptr);
	~MediaDeviceActivity();
	
public:
	void configure(QSharedPointer<Node> node);

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // MEDIADEVICEACTIVITY_H

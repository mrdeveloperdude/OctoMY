#ifndef CAMERAACTIVITY_HPP
#define CAMERAACTIVITY_HPP

#include "components/navigation/Activity.hpp"


namespace Ui {
class CameraActivity;
}


class CameraActivity : public Activity
{
	Q_OBJECT

private:
	Ui::CameraActivity *ui;

public:
	explicit CameraActivity(QWidget *parent = nullptr);
	~CameraActivity();
	
public:
	void configure();

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // CAMERAACTIVITY_HPP

#ifndef CONTROLBAPTISMACTIVITY_HPP
#define CONTROLBAPTISMACTIVITY_HPP

#include <QWidget>

#include "components/navigation/Activity.hpp"
#include "name/RemoteNameGenerator.hpp"

class Node;

namespace Ui {
class ControlBaptismActivity;
}

class ControlBaptismActivity : public Activity
{
	Q_OBJECT
private:
	Ui::ControlBaptismActivity *ui;
	RemoteNameGenerator mNameGenerator;
	QSharedPointer<Node> mNode;
	
	ConfigureHelper mConfigureHelper;
	
public:
	explicit ControlBaptismActivity(QWidget *parent = nullptr);
	~ControlBaptismActivity();
	
private:
	void configureValidators();
	QString name();
	
public:
	void configure(QSharedPointer<Node> mNode);
	void reset();
	
public slots:
	void randomizeName();
	void done();


	// Virtual activity mechanism
protected:
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
	void pushImpl(const QStringList arguments = QStringList()) override;
};

#endif // CONTROLBAPTISMACTIVITY_HPP

#ifndef VOICEACTIVITY_HPP
#define VOICEACTIVITY_HPP

#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Node;

namespace Ui {
class VoiceActivity;
}


class VoiceActivity : public Activity
{
	Q_OBJECT

private:
	Ui::VoiceActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit VoiceActivity(QWidget *parent = nullptr);
	~VoiceActivity();
	
public:
	void configure(QSharedPointer<Node> n);
	

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // VOICEACTIVITY_HPP

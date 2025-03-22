#ifndef WIDGETILLUSTRATIONACTIVITY_HPP
#define WIDGETILLUSTRATIONACTIVITY_HPP

#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Node;

namespace Ui {
class WidgetIllustrationActivity;
}


class WidgetIllustrationActivity : public Activity
{
	Q_OBJECT

private:
	Ui::WidgetIllustrationActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit WidgetIllustrationActivity(QWidget *parent = nullptr);
	~WidgetIllustrationActivity();
	
public:
	void configure(QSharedPointer<Node> n);
	

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // WIDGETILLUSTRATIONACTIVITY_HPP

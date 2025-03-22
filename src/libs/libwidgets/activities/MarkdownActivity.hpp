#ifndef MARKDOWNACTIVITY_HPP
#define MARKDOWNACTIVITY_HPP

#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Node;

namespace Ui {
class MarkdownActivity;
}


class MarkdownActivity : public Activity
{
	Q_OBJECT

private:
	Ui::MarkdownActivity *ui;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit MarkdownActivity(QWidget *parent = nullptr);
	~MarkdownActivity();
	
public:
	void configure(QSharedPointer<Node> n);
	

	// Virtual activity mechanism
protected:
	void pushImpl(const QStringList arguments = QStringList()) override;
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
};

#endif // MARKDOWNACTIVITY_HPP

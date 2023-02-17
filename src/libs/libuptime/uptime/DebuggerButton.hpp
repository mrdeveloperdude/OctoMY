#ifndef DEBUGGERBUTTON_HPP
#define DEBUGGERBUTTON_HPP

#include "uptime/ConfigureHelper.hpp"

#include <QPushButton>
#include <QSharedPointer>

class DebuggerWidget;
class Node;

namespace Ui
{
class DebuggerButton;
}

class DebuggerButton : public QPushButton
{
	Q_OBJECT
private:
	QSharedPointer<DebuggerWidget> mDebuggerWidget;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit DebuggerButton(QWidget *parent = nullptr);
	virtual ~DebuggerButton();

private:
	void setupUi();
	
	
public:
	void configure(QSharedPointer<Node> node, const bool startOpened=false);

private slots:
	void onToggled(bool checked);

};

#endif
// DEBUGGERBUTTON_HPP

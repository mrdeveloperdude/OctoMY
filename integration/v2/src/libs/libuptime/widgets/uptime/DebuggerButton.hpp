#ifndef DEBUGGERBUTTON_HPP
#define DEBUGGERBUTTON_HPP

#include "uptime/ConfigureHelper.hpp"

#include <QWidget>
#include <QSharedPointer>

class DebuggerWidget;
class Node;

namespace Ui
{
class DebuggerButton;
}

class DebuggerButton : public QWidget
{
	Q_OBJECT
private:
	Ui::DebuggerButton *ui;
	QSharedPointer<DebuggerWidget> mDebuggerWidget;
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;

public:
	explicit DebuggerButton(QWidget *parent = nullptr);
	virtual ~DebuggerButton();

public:
	void configure(QSharedPointer<Node> node);

private slots:
	void on_pushButtonShowDebugger_toggled(bool checked);

};

#endif
// DEBUGGERBUTTON_HPP

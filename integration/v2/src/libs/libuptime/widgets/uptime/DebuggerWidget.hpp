#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include "node/Node.hpp"

#include <QWidget>
#include <QSharedPointer>

namespace Ui
{
class Debugger;
}

class DebuggerWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::Debugger *ui;
	QSharedPointer<Node> mNode;

public:
	explicit DebuggerWidget(QWidget *parent = nullptr);
	~DebuggerWidget();

public:
	void configure(QSharedPointer <Node> node);

private slots:
	void on_pushButtonQuitSuccess_clicked();
	void on_pushButtonQuitFail_clicked();
	void on_pushButtonUnbirth_clicked();
	void on_pushButtonBirth_clicked();
};

#endif
// DEBUGGER_HPP

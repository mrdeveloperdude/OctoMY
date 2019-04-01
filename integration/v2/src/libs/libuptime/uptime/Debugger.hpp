#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include "node/Node.hpp"

#include <QWidget>
#include <QSharedPointer>

namespace Ui
{
class Debugger;
}

class Debugger : public QWidget
{
	Q_OBJECT
private:
	Ui::Debugger *ui;
	QSharedPointer<Node> mNode;

public:
	explicit Debugger(QWidget *parent = nullptr);
	~Debugger();

public:
	void configure(QSharedPointer <Node> node);

private slots:
	void on_pushButtonQuitSuccess_clicked();
	void on_pushButtonQuitFail_clicked();
};

#endif
// DEBUGGER_HPP

#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include "node/Node.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QWidget>
#include <QSharedPointer>

class QPushButton;

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
	QMap<QString, QWidget *> mHeaderWidgets;
	QList<QPushButton *> mHeaderButtons;
	ConfigureHelper mConfigureHelper;

public:
	explicit DebuggerWidget(QWidget *parent = nullptr);
	virtual ~DebuggerWidget() Q_DECL_OVERRIDE;

public:
	void configure(QSharedPointer <Node> node);
	void updateServiceTable();

private:
	void configureUi();
	void updateIdentity();

private:
	void birth();
	void unBirth();

	// QWidget interface
public:
	void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
	void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;

private slots:
	// Place directly to the right of nodewindow
	void tuck();
	// Compact to minimize size
	void pack();
	// Collapse/expand all
	void allEnable(bool enable);

private slots:
	void on_pushButtonQuitSuccess_clicked();
	void on_pushButtonQuitFail_clicked();
	void on_pushButtonTuckWindow_toggled(bool checked);
	void on_pushButtonRestart_clicked();
	void on_pushButtonExpandAll_clicked();
	void on_pushButtonCollapseAll_clicked();

signals:
	void visibilityChanged(bool visible);
};

#endif
// DEBUGGER_HPP

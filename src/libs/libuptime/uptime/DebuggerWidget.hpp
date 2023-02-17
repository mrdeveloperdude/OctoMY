#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include "node/Node.hpp"
#include "uptime/ConfigureHelper.hpp"

#include <QWidget>
#include <QSharedPointer>
#include <QEnableSharedFromThis>

class QPushButton;

namespace Ui
{
class Debugger;
}

struct EnabledCount {
	int total;
	int enabled;
	EnabledCount():total(0), enabled(0)
	{

	}
	void count(bool e)
	{
		total++;
		if(e) {
			enabled++;
		}
	}
	bool allIs(bool e) const
	{
		return e?(total==enabled):(0==enabled);
	}
};

class DebuggerWidget : public QWidget, public QEnableSharedFromThis<DebuggerWidget>
{
	Q_OBJECT
private:
	Ui::Debugger *ui;
	QSharedPointer<Node> mNode;
	QMap<QString, QWidget *> mHeaderWidgets;
	QList<QPushButton *> mHeaderButtons;
	ConfigureHelper mConfigureHelper;
	QMetaObject::Connection mTuckNodeWindowConnection;
	QMetaObject::Connection mTuckDebuggerWindowConnection;

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
	void moveEvent(QMoveEvent *event) Q_DECL_OVERRIDE;

private slots:
	// Place directly to the right of nodewindow
	void tuck(bool attach=true, QWidget *movedWindow=nullptr);
	// Compact to minimize size
	void pack();
	// Collapse/expand all
	void allEnable(bool enable);
	// Number of collapsed/expanded
	EnabledCount numEnabled();
	// Update expand/collapse button with correct icon
	void updateExpandButton();

private slots:
	void on_pushButtonQuitSuccess_clicked();
	void on_pushButtonQuitFail_clicked();
	void on_pushButtonTuckWindow_toggled(bool checked);
	void on_pushButtonRestart_clicked();
	void on_pushButtonExpandOrCollapseAll_clicked();

signals:
	void visibilityChanged(bool visible);
	void debuggerWindowMoved(QSharedPointer <DebuggerWidget> window);

};

#endif
// DEBUGGER_HPP

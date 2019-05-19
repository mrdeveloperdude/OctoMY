#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include "node/Node.hpp"

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

public:
	explicit DebuggerWidget(QWidget *parent = nullptr);
	virtual ~DebuggerWidget() Q_DECL_OVERRIDE;

public:
	void configure(QSharedPointer <Node> node);
	void updateServiceTable();

private:
	void configureUi();
	void updateIdentity();

	// QWidget interface
public:
	void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
	void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;

private slots:
	// Place directly to the right of nodewindow
	void tuck();
	// Compact to minimize size
	void pack();

private slots:
	void on_pushButtonQuitSuccess_clicked();
	void on_pushButtonQuitFail_clicked();
	void on_pushButtonUnbirth_clicked();
	void on_pushButtonBirth_clicked();

	void on_pushButtonTuckWindow_toggled(bool checked);

	void on_pushButtonRestart_clicked();

	signals:
	void visibilityChanged(bool visible);
};

#endif
// DEBUGGER_HPP

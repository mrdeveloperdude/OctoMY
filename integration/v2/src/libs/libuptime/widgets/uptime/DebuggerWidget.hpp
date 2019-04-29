#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include "node/Node.hpp"

#include <QWidget>
#include <QSharedPointer>

class QTableWidgetItem;

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
	virtual ~DebuggerWidget() Q_DECL_OVERRIDE;

public:
	void configure(QSharedPointer <Node> node);

private:
	void setServiceTableItem(const int index, const QString serviceName, const bool expected, const bool actual);
	QTableWidgetItem *tableItem(const bool s);
	QTableWidgetItem *tableItem(const QString s);
	void updateServiceTable();

	// QWidget interface
public:
	void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
	void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;

private slots:
	void on_pushButtonQuitSuccess_clicked();
	void on_pushButtonQuitFail_clicked();
	void on_pushButtonUnbirth_clicked();
	void on_pushButtonBirth_clicked();
	void on_pushButtonActivate_toggled(bool checked);

signals:
	void visibilityChanged(bool visible);
};

#endif
// DEBUGGER_HPP

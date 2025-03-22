#ifndef COMMANDWIDGET_HPP
#define COMMANDWIDGET_HPP

#include <QMap>
#include <QStringList>
#include <QWidget>
#include "uptime/SharedPointerWrapper.hpp"

namespace Ui {
class CommandWidget;
}

class Command;

class CommandWidget : public QWidget {
	Q_OBJECT
private:
	Ui::CommandWidget *ui;
	int currentHistoryIndex{-1};
	QStringList commandHistory;
	QMap<QString, QSharedPointer<Command>> commandRegistry;
	QStringList currentSuggestions;
	int currentSuggestionIndex{-1}; 
	
	
public:
	explicit CommandWidget(QWidget *parent = nullptr);
	~CommandWidget();

private slots:
	void executeCommand();

	// QWidget interface
protected:
	void keyPressEvent(QKeyEvent *event) override;
	void focusInEvent(QFocusEvent *event) override;
	bool eventFilter(QObject *watched, QEvent *event) override;

private:
	void addCommandToHistory(const QString &command);
	void addResultToHistory(const QString &result);
	void updateCommandLine(const QString &command);
	void scrollToBottom();
	
	// Commands
private:
	void registerCommand(QSharedPointer<Command> command);

	// Completion
private:
	void appendSuggestionToLineEdit(const QString &suggestion, bool select = true);
	void handleTabCompletion();
	QStringList suggestCommands(const QString &partialCommand);
	void showTabPreview(const QStringList &suggestions);
	void clearTabPreview();
};

#endif // COMMANDWIDGET_HPP

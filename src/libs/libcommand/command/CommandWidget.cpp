#include "CommandWidget.hpp"
#include "ui_CommandWidget.h"

#include <QScrollBar>
#include <QKeyEvent>
#include <QDir>


#include "Command.hpp"
#include "commands/DockerCommand.hpp"
#include "commands/LsCommand.hpp"

CommandWidget::CommandWidget(
	QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::CommandWidget)
{
	ui->setupUi(this);
	ui->lineEditCommand->setFocus();
	connect(ui->lineEditCommand, &QLineEdit::returnPressed, this, &CommandWidget::executeCommand);
	ui->lineEditCommand->installEventFilter(this);
	
	registerCommand(QSharedPointer<Command>(new DockerCommand()));
	registerCommand(QSharedPointer<Command>(new LsCommand()));
}


CommandWidget::~CommandWidget()
{
	delete ui;
}


void CommandWidget::registerCommand(QSharedPointer<Command> command) {
	commandRegistry[command->name()] = command;
}


bool CommandWidget::eventFilter(QObject *watched, QEvent *event) {
	if (watched == ui->lineEditCommand && event->type() == QEvent::KeyPress && ui->lineEditCommand->hasFocus()) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if (keyEvent->key() == Qt::Key_Tab) {
			handleTabCompletion();
			return true;
		}
	}
	return QWidget::eventFilter(watched, event);
}

void CommandWidget::executeCommand() {
	const QString command = ui->lineEditCommand->text().trimmed();
	if (command.isEmpty()) {
		return;
	}
	ui->scrollAreaWidgetContents->setUpdatesEnabled(false);
	addCommandToHistory(command);
	QString result = QString("Result of \"%1\"").arg(command);
	addResultToHistory(result);
	ui->lineEditCommand->clear();
	currentHistoryIndex = -1;
	currentSuggestions.clear();
	currentSuggestionIndex = -1;
	scrollToBottom();
	ui->scrollAreaWidgetContents->setUpdatesEnabled(true);
}


void CommandWidget::addCommandToHistory(const QString &command) {
	commandHistory.append(command);
	QLabel *commandLabel = new QLabel(command, ui->scrollAreaWidgetContents);
	commandLabel->setStyleSheet("color: blue; font-weight: bold; margin: 0; padding: 0;");
	commandLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
	commandLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
	commandLabel->setWordWrap(true);
	ui->verticalLayoutHistory->addWidget(commandLabel);
}

void CommandWidget::addResultToHistory(const QString &result) {
	QLabel *resultLabel = new QLabel(result, ui->scrollAreaWidgetContents);
	resultLabel->setStyleSheet("color: green; margin: 0; padding: 0;");
	resultLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
	resultLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
	resultLabel->setWordWrap(true);
	ui->verticalLayoutHistory->addWidget(resultLabel);
}

void CommandWidget::keyPressEvent(QKeyEvent *event) {
	switch (event->key()) {
	case Qt::Key_Up:
		if (!commandHistory.isEmpty() && currentHistoryIndex > 0) {
			--currentHistoryIndex;
			updateCommandLine(commandHistory[currentHistoryIndex]);
		}
		break;
	case Qt::Key_Down:
		if (!commandHistory.isEmpty() && currentHistoryIndex < commandHistory.size() - 1) {
			++currentHistoryIndex;
			updateCommandLine(commandHistory[currentHistoryIndex]);
		} else if (currentHistoryIndex == commandHistory.size() - 1) {
			++currentHistoryIndex;
			ui->lineEditCommand->clear();
		}
		break;
	default:
		QWidget::keyPressEvent(event);
		break;
	}
}

void CommandWidget::focusInEvent(QFocusEvent *event){
	Q_UNUSED(event);
	ui->lineEditCommand->setFocus();
}

void CommandWidget::updateCommandLine(const QString &command) {
	ui->lineEditCommand->setText(command);
	ui->lineEditCommand->setCursorPosition(command.length());
}

void CommandWidget::scrollToBottom() {
	QScrollBar *scrollBar = ui->scrollAreaHistory->verticalScrollBar();
	if (scrollBar) {
		scrollBar->setValue(scrollBar->maximum());
	}
}


void CommandWidget::appendSuggestionToLineEdit(const QString &suggestion, bool select) {
	QString input = ui->lineEditCommand->text();
	QStringList parts = input.split(' ', Qt::SkipEmptyParts);
	QString commandName = parts.value(0);
	QString args = parts.mid(1).join(' ');
	
	QString newCommand;
	if (parts.size() > 1) {
		newCommand = QString("%1 %2").arg(commandName, suggestion);
	} else {
		newCommand = suggestion;
	}
	
	ui->lineEditCommand->setText(newCommand);
	if(select){
		ui->lineEditCommand->setCursorPosition(commandName.length() + 1);
		ui->lineEditCommand->setSelection(commandName.length() + 1, suggestion.length());
	}
	else{
		ui->lineEditCommand->setCursorPosition(newCommand.length() + 1);
		ui->lineEditCommand->setSelection(0, 0);
	}
}


void CommandWidget::handleTabCompletion() {
	const QString input = ui->lineEditCommand->text().trimmed();
	if (input.isEmpty()) {
		return;
	}
	QStringList parts = input.split(' ', Qt::SkipEmptyParts);
	const QString commandName = parts.value(0);
	const QString args = parts.mid(1).join(' ');
	QStringList suggestions;
	if (!commandRegistry.contains(commandName)) {
		suggestions = suggestCommands(commandName);
	} else {
		suggestions = commandRegistry[commandName]->complete(args);
	}
	
	if (currentSuggestions.isEmpty()) {
		currentSuggestions = commandRegistry.keys(); // List all available commands
		showTabPreview(currentSuggestions);
		currentSuggestionIndex = -1; // Reset index
		return;
	}
	
	// Handle single suggestion
	if (currentSuggestions.size() == 1) {
		appendSuggestionToLineEdit(currentSuggestions.first());
		currentSuggestions.clear();
		currentSuggestionIndex = -1;
		return;
	}
	
	// Cycle through suggestions
	currentSuggestionIndex = (currentSuggestionIndex + 1) % currentSuggestions.size();
	appendSuggestionToLineEdit(currentSuggestions[currentSuggestionIndex]);
	
	
	//showTabPreview(suggestions);
}

QStringList CommandWidget::suggestCommands(const QString &partialCommand) {
	QStringList suggestions;
	for (const auto &commandName : commandRegistry.keys()) {
		if (commandName.startsWith(partialCommand, Qt::CaseSensitive)) {
			suggestions.append(commandName);
		}
	}
	return suggestions;
}

void CommandWidget::showTabPreview(const QStringList &suggestions) {
	clearTabPreview();
	
	if (suggestions.isEmpty()) {
		addResultToHistory("Available commands: " + commandRegistry.keys().join(", "));
		return;
	}
	
	// Display the suggestions as a single label
	QString suggestionText = "Suggestions: " + suggestions.join(", ");
	QLabel *previewLabel = new QLabel(suggestionText, ui->scrollAreaWidgetContents);
	previewLabel->setStyleSheet("color: gray; font-style: italic;");
	previewLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
	previewLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
	ui->verticalLayoutHistory->addWidget(previewLabel);
	
	// Scroll to the bottom
	scrollToBottom();
}

void CommandWidget::clearTabPreview() {
	// Remove the last tab preview if any (optional, based on your desired behavior)
}

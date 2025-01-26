#include "SplitterWidget.hpp"

#include <QApplication>
#include <QDebug>
#include <QShortcut>
#include <QSplitter>
#include <QVBoxLayout>

#include "SplitterPane.hpp"

SplitterWidget::SplitterWidget(QWidget *parent)
	: QWidget(parent), mainSplitter(new QSplitter(Qt::Horizontal, this)) {
	auto layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	mainSplitter->setChildrenCollapsible(false);
	layout->addWidget(mainSplitter);
	setupShortcuts();
	addInitialPane();
}

SplitterWidget::~SplitterWidget() = default;

void SplitterWidget::addInitialPane() {
	auto initialPane = new SplitterPane(mainSplitter, this);
	mainSplitter->addWidget(initialPane);
	setFocusedPane(initialPane);
}


void SplitterWidget::setupShortcuts() {
	shortcutSplitHorizontal = new QShortcut(QKeySequence("Ctrl+T"), this);
	connect(shortcutSplitHorizontal, &QShortcut::activated, this, &SplitterWidget::splitCurrentPaneHorizontally);
	shortcutSplitVertical = new QShortcut(QKeySequence("Ctrl+Shift+T"), this);
	connect(shortcutSplitVertical, &QShortcut::activated, this, &SplitterWidget::splitCurrentPaneVertically);
	shortcutClosePane = new QShortcut(QKeySequence("Ctrl+W"), this);
	connect(shortcutClosePane, &QShortcut::activated, this, &SplitterWidget::closeCurrentPane);
}


void SplitterWidget::setFocusedPane(SplitterPane *pane) {
	if(pane != currentPane){
		if(nullptr != currentPane){
			currentPane->setPaneFocused(false);
		}
		// Triggers dangling pointer bug so commented out: qDebug() << "Focused pane updated " << currentPane<< " --> "<< pane;
		currentPane = pane;
		if(nullptr != currentPane){
			currentPane->setPaneFocused(true);
		}
	}
	else{
		qDebug() << "No change in focus" << currentPane;
	}
}

SplitterPane* SplitterWidget::focusedPane() const {
	return currentPane;
}

void SplitterWidget::splitCurrentPaneHorizontally() {
	auto pane = focusedPane();
	if (pane) {
		qDebug() << "Splitting horizontally";
		auto newPane = pane->splitPaneHorizontally();
		if(nullptr != newPane){
			setFocusedPane(newPane);
		}
	} else {
		qWarning() << "No focused pane to split horizontally.";
	}
}

void SplitterWidget::splitCurrentPaneVertically() {
	auto pane = focusedPane();
	if (pane) {
		qDebug() << "Splitting vertically";
		auto newPane = pane->splitPaneVertically();
		if(nullptr != newPane){
			setFocusedPane(newPane);
		}
	} else {
		qWarning() << "No focused pane to split vertically.";
	}
}

void SplitterWidget::closeCurrentPane() {
	auto pane = focusedPane();
	if (pane) {
		qDebug() << "Closing";
		pane->closePane();
	} else {
		qWarning() << "No focused pane to close.";
	}
}


bool SplitterWidget::eventFilter(QObject *watched, QEvent *event) {
	return QWidget::eventFilter(watched, event);
}

#ifndef SPLITTERWIDGET_HPP
#define SPLITTERWIDGET_HPP

#include <QWidget>

class QSplitter;
class QShortcut;
class SplitterPane;

class SplitterWidget : public QWidget {
	Q_OBJECT
private:
	QSplitter *mainSplitter{nullptr};
	QShortcut *shortcutSplitHorizontal{nullptr};
	QShortcut *shortcutSplitVertical{nullptr};
	QShortcut *shortcutClosePane{nullptr};
	SplitterPane *currentPane{nullptr};

public:
	explicit SplitterWidget(QWidget *parent = nullptr);
	~SplitterWidget();
	
private:
	void setupShortcuts();
	
public:
	SplitterPane* focusedPane() const;
	void addInitialPane(); // Add the initial pane
	void splitCurrentPaneHorizontally();
	void splitCurrentPaneVertically();
	void closeCurrentPane();
	
protected:
	bool eventFilter(QObject *watched, QEvent *event) override;

public slots:
	void setFocusedPane(SplitterPane *pane);

};

#endif // SPLITTERWIDGET_HPP

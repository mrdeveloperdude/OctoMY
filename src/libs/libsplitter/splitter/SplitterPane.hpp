#ifndef SPLITTERPANE_HPP
#define SPLITTERPANE_HPP

#include <QWidget>

class QSplitter;
class SplitterWidget;

namespace Ui {
class SplitterPane;
}

enum Edge {
	  Top
	, Bottom
	, Left
	, Right
	, None
};


QString EdgeToString(const Edge type);
QDebug operator<<(QDebug debug, const Edge &type);


class SplitterPane : public QWidget
{
	Q_OBJECT
private:
	Ui::SplitterPane *ui{nullptr};
	SplitterWidget *splitterWidget{nullptr};
	QPoint dragStartPosition;
	bool mPanelFocused{false};
	Edge mEdge{None};
	QMenu *mMenu{nullptr};

public:
	explicit SplitterPane(QWidget *parent = nullptr, SplitterWidget *splitterWidget = nullptr);
	~SplitterPane();

private:
	SplitterPane *detachPane();
	bool movePane(SplitterPane *targetPane, Edge targetDropEdge);
	SplitterPane *splitPane(Qt::Orientation orientation);
	Qt::Orientation orientation() const;
	QSplitter* parentSplitter() const;
	void setPaneFocused(bool focused);
	void setFrameStyle(const QString &style);
	void updateFocusVisual();
	void updateDropIndicator(const QPoint &mousePos);
	void setContentWidget(QWidget *newWidget);
	void buildMenu();
	
private slots:
	SplitterPane *splitPaneVertically();
	SplitterPane *splitPaneHorizontally();
	void arrangeSplitter();
	void closePane();
	void startDrag();
	void showMenu();

protected:
	bool eventFilter(QObject *watched, QEvent *event) override;
	void dragEnterEvent(QDragEnterEvent *event) override;
	void dragLeaveEvent(QDragLeaveEvent *event) override;
	void dragMoveEvent(QDragMoveEvent *event) override;
	void dropEvent(QDropEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;

signals:
	void paneRequestFocus(SplitterPane *pane);
		
	friend class SplitterWidget;
};

#endif // SPLITTERPANE_HPP


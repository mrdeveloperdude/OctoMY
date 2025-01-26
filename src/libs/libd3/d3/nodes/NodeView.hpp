#ifndef NODEVIEW_H
#define NODEVIEW_H

#include <QBrush>
#include <QFontDatabase>
#include <QMap>
#include <QMenu>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <QSharedPointer>
#include <QString>
#include <QVector>
#include <QWidget>


class Node;
class Connection;
class Tri;
class QSettings;
class Graph;
class RenameDialog;
class ConnectionParameterWidget;
class NodeParameterWidget;
class TriParameterWidget;

void antialiasHints(QPainter &painter);



enum DragMode{
	OFF, CONNECT_NODE, MOVE_NODE, PAN_VIEW, LASSO
};

class NodeView : public QWidget
{
	Q_OBJECT
private:
	QMenu *mSelectNodeMenu{nullptr};
	QMenu *mNodeContextMenu{nullptr};
	QMenu *mConnectionContextMenu{nullptr};
	QMenu *mTriContextMenu{nullptr};
	RenameDialog * renameDialogue{nullptr};
	QSharedPointer<Graph> mGraph;
	QSharedPointer<QSettings> mSettings;
	QPointF mDragStartPos;
	QPointF mDragEndPos;
	QPointF mDragLastPos;
	QPointF mDragOffsetPos;
	QPointF mNodeViewPan;
	QSharedPointer<Node> mSourceNode;
	QSharedPointer<Node> mDestinationNode;
	QVector<QSharedPointer<Node> >mSelection;
	QMap<QSharedPointer<Node>, QSharedPointer<NodeParameterWidget> > mNodeParameterWidgets;
	QMap<QSharedPointer<Connection>, QSharedPointer<ConnectionParameterWidget> > mConnectionParameterWidgets;
	QMap<QSharedPointer<Tri>, QSharedPointer<TriParameterWidget> > mTriParameterWidgets;
	DragMode dragMode{OFF};

	bool mXray{false};
	bool mDebug{false};

public:
	explicit NodeView(QWidget *parent = nullptr);

private:
	void addNodeToSelectNodeMenu(QString name);
	void updateSelectNodeMenu();
	void updateNodeContextMenu(QSharedPointer<Node> node);
	void updateConnectionContextMenu(QSharedPointer<Connection> connection);
	void updateTriContextMenu(QSharedPointer<Tri> tri);
	void invalidateLinesToNode(QSharedPointer<Node> node);
	void syncSelection(bool selected);
	void invertSelection();
	void flipSelection();
	void interConnectSelected();
	void interTriSelected();
	void removeSelected();
	
	QSharedPointer<NodeParameterWidget> nodeParameterWidget(QSharedPointer<Node> node);
	QSharedPointer<ConnectionParameterWidget> connectionParameterWidget(QSharedPointer<Connection> connection);
	QSharedPointer<TriParameterWidget> triParameterWidget(QSharedPointer<Tri> tri);

public:
	void setGraph(QSharedPointer<Graph> graph);
	QSharedPointer<Graph> graph();
	void setSettings(QSharedPointer<QSettings> settings);
	void zoomToFit(const QRectF &rect);
	void autoArrange(const QRectF &target);
	void toggleXRay(bool xray_on);


protected:
	//void resizeEvent(QResizeEvent *) override;
	void paintEvent(QPaintEvent *) override;
	void mousePressEvent(QMouseEvent *) override;
	void mouseMoveEvent(QMouseEvent *) override;
	void mouseReleaseEvent(QMouseEvent *) override;
	void mouseDoubleClickEvent(QMouseEvent *) override;
	void wheelEvent(QWheelEvent *event) override;
	void closeEvent(QCloseEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;

public slots:
	void addNode(QPointF pos, QString name);
signals:
	void navigate(QString destination);

};

#endif // NODEVIEW_H


#ifndef GRAPH_H
#define GRAPH_H

#include "Style.hpp"

#include <QPainter>
#include <QPoint>
#include <QSharedPointer>
#include <QVector>

class Tri;
class Connection;
class Node;
class Project;
class QRectF;


enum LassoType{
	Intersection, CenterIn, RectIn
};


enum NodeSelector{
	MUST_INCLUDE, MUST_NOT_INCLUDE, EITHER_IS_FINE
};


class ConnectionStatus{
public:
	const bool ok;
	const QString reason;
public:
	ConnectionStatus(const bool ok, const QString &reason=""):
		ok(ok), reason(reason){
	}
};


////////////////////////////////////////////////////////////////////////

class Graph: public QEnableSharedFromThis<Graph>{
private:
	QSharedPointer<Project> mProject;
	QVector<QSharedPointer<Node> > mNodes;
	QVector<QSharedPointer<Connection> > mConnections;
	QVector<QSharedPointer<Tri> > mTris;
	bool mTopologyChanged{true};

	bool mAllowCycles{true};
	bool mDebug{false};

private:
	bool hasCycleWorker(QSharedPointer<Node> node, QSet<QString> &visited, QSet<QString> &recStack, const QSharedPointer<Node> &nodeA, const QSharedPointer<Node> &nodeB) const;
	
	
	void taint(const QString &reason);
public:
	Graph(QSharedPointer<Project> project = nullptr);
	
public:
	QSharedPointer<Project> project() {return mProject;}
	
// Data structure
public:
	bool clear();
	ConnectionStatus addConnection(QSharedPointer<Node> fromNode,QSharedPointer<Node> toNode);
	ConnectionStatus addTri(QSharedPointer<Node> n1, QSharedPointer<Node> n2, QSharedPointer<Node> n3);
	QSharedPointer<Connection> connectionAt(const QPointF &pt, const qreal radius = 20.0) const;
	QSharedPointer<Tri> triAt(const QPointF &pt) const;
	QSharedPointer<Node> nodeAt(const QPointF &pt) const;
	QSharedPointer<Node> nodeByID(const QString &id) const;
	QSharedPointer<Node> nodeByName(const QString &name) const;
	QVector< QSharedPointer<Connection> >connectionsForNode(QSharedPointer<Node> node, NodeSelector fromSelector=EITHER_IS_FINE, NodeSelector toSelector=EITHER_IS_FINE) const;
	QVector<QSharedPointer<Connection> > connections() const;
	QVector<QSharedPointer<Node> > nodes() const;
	QVector<QSharedPointer<Node> > nodesOfType(const QString &type) const;
	QVector<QSharedPointer<Node> > nodesAt(const QRectF &lasso, const LassoType lassoType = RectIn, QSharedPointer<Node> exclude=nullptr) const;
	QSharedPointer<Connection> connectionFor(QSharedPointer<Node> fromNode,QSharedPointer<Node> toNode, bool directionMatters=false) const;
	QSharedPointer<Tri> triFor(const QSharedPointer<Node> &n1, const QSharedPointer<Node> &n2, const QSharedPointer<Node> &n3, bool orderMatters=false, bool directionMatters=false) const;
	bool connectionExists(QSharedPointer<Node> fromNode,QSharedPointer<Node> toNode, bool directionMatters=false) const;
	bool triExists(QSharedPointer<Node> n1, QSharedPointer<Node> n2, QSharedPointer<Node> n3, bool directionMatters=false) const;
	bool wouldCreateCycle(const QSharedPointer<Node> &nodeA, const QSharedPointer<Node> &nodeB) const;
	inline bool topologyChanged() const;
	inline void resetTopologyChanged();

	inline qsizetype connectionCount() const;
	inline qsizetype nodeCount() const;
	void addNode(QSharedPointer<Node> node);
	void disconnectNode(QSharedPointer<Node> node);
	void disTriNode(QSharedPointer<Node> node);
	void removeTri(QSharedPointer<Tri> tri);
	void removeConnection(QSharedPointer<Connection> connection);
	void removeNode(QSharedPointer<Node> node);
	
	
	void step(qreal dt);
	
// Visualization
public:
	void drawStats(QPainter &painter, const Style &style) const;
	void draw(QPainter &painter, const bool xray, const Style &style) const;
	void zoomToFit(const QRectF &target);

	// Load/Save streams
public:
	friend QDataStream& operator<<(QDataStream& out, const Graph& obj);
	friend QDataStream& operator>>(QDataStream& in, Graph& obj);
};



qsizetype Graph::nodeCount() const{
	return mNodes.size();
}


qsizetype Graph::connectionCount() const{
	return mConnections.size();
}


bool Graph::topologyChanged() const{
	return mTopologyChanged;
};


void Graph::resetTopologyChanged() {
	mTopologyChanged = false;
};


#endif // GRAPH_H

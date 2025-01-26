#ifndef CONNECTION_H
#define CONNECTION_H

#include <QDataStream>
#include <QSharedPointer>

class Graph;
class Node;
class QPainter;
struct Style;

class Connection{
private:
	QSharedPointer<Graph> mGraph;
	QSharedPointer<Node> mFrom;
	QSharedPointer<Node> mTo;
	qreal mRestLength{0.0};
	qreal mForce{0.0};
	bool mLine_invalid{true};
	
public:
	Connection() = default;
	Connection(QSharedPointer<Graph> graph, QSharedPointer<Node> from, QSharedPointer<Node> to);

public:
	QSharedPointer<Graph> graph() const;
	QSharedPointer<Node> from() const;
	QSharedPointer<Node> to() const;
	
	void relax();
	void setForce(const qreal force);
	
	void taintLine();
	void unTaintLine();
	
	QString name() const;
	
public:
	void draw(QPainter &painter, const Style &style);

public:
	friend QDataStream& operator<<(QDataStream& out, const Connection& obj);
	friend QDataStream& operator>>(QDataStream& in, Connection& obj);

};
#endif // CONNECTION_H

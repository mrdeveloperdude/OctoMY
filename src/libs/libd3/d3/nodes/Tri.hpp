#ifndef TRI_HPP
#define TRI_HPP

#include <QDataStream>
#include "uptime/SharedPointerWrapper.hpp"

class Graph;
class Node;
class QPainter;
struct Style;

class Tri{
private:
	QSharedPointer<Graph> mGraph;
	QSharedPointer<Node> mN1;
	QSharedPointer<Node> mN2;
	QSharedPointer<Node> mN3;
	
public:
	Tri() = default;
	Tri(QSharedPointer<Graph> graph=nullptr, QSharedPointer<Node> n1=nullptr, QSharedPointer<Node> n2=nullptr, QSharedPointer<Node> n3=nullptr);
	
public:
	QSharedPointer<Graph> graph() const;
	QSharedPointer<Node> node1() const;
	QSharedPointer<Node> node2() const;
	QSharedPointer<Node> node3() const;
	QString name() const;
	
public:
	void draw(QPainter &painter, const Style &style);
	
public:
	friend QDataStream& operator<<(QDataStream& out, const Tri& obj);
	friend QDataStream& operator>>(QDataStream& in, Tri& obj);
	
};

#endif // TRI_HPP

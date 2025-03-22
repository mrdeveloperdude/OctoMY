#ifndef GEOMETRYENGINE_HPP
#define GEOMETRYENGINE_HPP

#include <QPointF>
#include "uptime/SharedPointerWrapper.hpp"

class Graph;
class Node;
class Tri;

struct Triangle {
	QPointF a, b, c;
	
	// Overload operator== for comparison
	bool operator==(const Triangle &other) const {
		return (a == other.a && b == other.b && c == other.c) ||
			   (a == other.a && b == other.c && c == other.b) ||
			   (a == other.b && b == other.a && c == other.c) ||
			   (a == other.b && b == other.c && c == other.a) ||
			   (a == other.c && b == other.a && c == other.b) ||
			   (a == other.c && b == other.b && c == other.a);
	}
};


class GeometryEngine
{
public:
	GeometryEngine();
	
	void interTri(QSharedPointer<Graph> graph, QList<QSharedPointer<Node> > nodes);
	void interConnect(QSharedPointer<Graph> graph, QList<QSharedPointer<Node> > nodes);
	
	void relax(QSharedPointer<Graph> graph, QList<QSharedPointer<Node> > nodes);

};

#endif // GEOMETRYENGINE_HPP

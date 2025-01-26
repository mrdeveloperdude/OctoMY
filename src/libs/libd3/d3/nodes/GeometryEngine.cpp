#include "GeometryEngine.hpp"

#include "Graph.hpp"
#include "Node.hpp"
#include "Connection.hpp"
#include "Tri.hpp"

#include <QVector>


static inline bool isPointInCircumcircle(const QPointF &p, const QPointF &a, const QPointF &b, const QPointF &c) {
	// Matrix determinant method to check circumcircle inclusion
	double ax = a.x() - p.x();
	double ay = a.y() - p.y();
	double bx = b.x() - p.x();
	double by = b.y() - p.y();
	double cx = c.x() - p.x();
	double cy = c.y() - p.y();
	
	double det = (ax * (by * (cx * cx + cy * cy) - cy * (bx * bx + by * by)) -
				  ay * (bx * (cx * cx + cy * cy) - cx * (bx * bx + by * by)) +
				  (ax * ax + ay * ay) * (bx * cy - by * cx));
	
	return det > 0;
}

static inline QVector<Triangle> performDelaunayTriangulation(const QVector<QPointF> &points) {
	
	QVector<Triangle> triangles;
	
	// Add a super-triangle that encompasses all points
	QPointF pMin = points[0], pMax = points[0];
	for (const auto &p : points) {
		pMin.setX(qMin(pMin.x(), p.x()));
		pMin.setY(qMin(pMin.y(), p.y()));
		pMax.setX(qMax(pMax.x(), p.x()));
		pMax.setY(qMax(pMax.y(), p.y()));
	}
	qreal pad{100.0};
	QPointF superA(pMin.x() - pad, pMin.y() - pad);
	QPointF superB(pMax.x() + pad, pMin.y() - pad);
	QPointF superC((pMin.x() + pMax.x()) / 2, pMax.y() + pad);
	triangles.append({superA, superB, superC});
	
	// Incrementally add points to the triangulation
	for (const auto &p : points) {
		QVector<Triangle> badTriangles;
		QVector<QPair<QPointF, QPointF>> polygonEdges;
		
		// Find all triangles whose circumcircle contains the point
		for (const auto &tri : triangles) {
			if (isPointInCircumcircle(p, tri.a, tri.b, tri.c)) {
				badTriangles.append(tri);
				
				// Add edges to polygon (but don't duplicate)
				polygonEdges.append({tri.a, tri.b});
				polygonEdges.append({tri.b, tri.c});
				polygonEdges.append({tri.c, tri.a});
			}
		}
		
		// Remove bad triangles
		for (const auto &tri : badTriangles) {
			triangles.removeAll(tri);
		}
		
		// Remove duplicate edges (edges that appear twice)
		for (qsizetype i = 0; i < polygonEdges.size(); ++i) {
			for (qsizetype j = i + 1; j < polygonEdges.size(); ++j) {
				if ((polygonEdges[i].first == polygonEdges[j].second &&
					 polygonEdges[i].second == polygonEdges[j].first)) {
					polygonEdges.remove(j);
					polygonEdges.remove(i);
					--i; // Recheck current index
					break;
				}
			}
		}
		
		// Create new triangles with the point and remaining polygon edges
		for (const auto &edge : polygonEdges) {
			triangles.append({edge.first, edge.second, p});
		}
	}
	
	// Remove triangles that share a vertex with the super-triangle
	triangles.erase(
		std::remove_if(triangles.begin(), triangles.end(), [&](const Triangle &tri) {
			return tri.a == superA || tri.a == superB || tri.a == superC ||
				   tri.b == superA || tri.b == superB || tri.b == superC ||
				   tri.c == superA || tri.c == superB || tri.c == superC;
		}),
		triangles.end());
	return triangles;
	
	
}




GeometryEngine::GeometryEngine() {}


void GeometryEngine::interTri(QSharedPointer<Graph> graph, QList<QSharedPointer<Node> > nodes){
	if (nodes.size() < 3) {
		return; // Not enough points to form a triangle
	}
	// Trivial case, handled separately to make sure it completes instead of getting stuck in an edge case in the triangulation algorithm
	if (nodes.size() == 3) {
		auto n1 = nodes[0];
		auto n2 = nodes[1];
		auto n3 = nodes[2];
		if(!graph->triExists(n1, n2, n3, false)){
			graph->addTri(n1, n2, n3);
		}
		return;
	}
	
	// Collect positions of selected nodes
	QVector<QPointF> points;
	for (const auto &node : nodes) {
		points.append(node->pos());
	}
	
	
	const auto triangles = performDelaunayTriangulation(points);
	
	// Add the resulting triangles to the graph
	for (const auto &tri : triangles) {
		auto n1 = nodes[points.indexOf(tri.a)];
		auto n2 = nodes[points.indexOf(tri.b)];
		auto n3 = nodes[points.indexOf(tri.c)];
		if (!graph->triExists(n1, n2, n3, false)) {
			graph->addTri(n1, n2, n3);
		}
	}
}

void GeometryEngine::interConnect(QSharedPointer<Graph> graph, QList<QSharedPointer<Node> > nodes){
	if (nodes.size() < 2) {
		return;
	}
	// Trivial case, handled separately to make sure it completes instead of getting stuck in an edge case in the triangulation algorithm
	if (nodes.size() == 2) {
		auto from = nodes[0];
		auto to = nodes[1];
		if(!graph->connectionExists(from, to, false)){
			graph->addConnection(from, to);
		}
		return;
	}
	
	// Collect positions of selected nodes
	QVector<QPointF> points;
	for (const auto &node : nodes) {
		points.append(node->pos());
	}
	
	const auto triangles = performDelaunayTriangulation(points);
	
	// Add the edges of the resulting triangles as connections to the graph
	for (const auto &tri : triangles) {
		auto n1 = nodes[points.indexOf(tri.a)];
		auto n2 = nodes[points.indexOf(tri.b)];
		auto n3 = nodes[points.indexOf(tri.c)];
		
		if(!graph->connectionExists(n1, n2, false)){
			graph->addConnection(n1, n2);
		}
		if(!graph->connectionExists(n2, n3, false)){
			graph->addConnection(n2, n3);
		}
		if(!graph->connectionExists(n3, n1, false)){
			graph->addConnection(n3, n1);
		}
	}
}






/*
 



void NodeView::interConnectSelected(){
	const auto size = mSelection.size();
	bool dirty{false};
	for(qsizetype i = 0; i< size; ++i){
		const auto from = mSelection[i];
		if(from->selected()){
			for(qsizetype j = i+1; j< size; ++j){
				const auto to = mSelection[j];
				if(to->selected()){
					if(!mGraph->areConnected(from, to, false)){
						mGraph->addConnection(from, to);
						dirty = true;
					}
				}
			}
		}
	}
	if(dirty){
		update();
	}
}

 
void NodeView::interTriSelected(){
	const auto size = mSelection.size();
	bool dirty{false};
	for(qsizetype i = 0; i < size; ++i){
		const auto n1 = mSelection[i];
		if(n1->selected()){
			for(qsizetype j = i + 1; j < size; ++j){
				const auto n2 = mSelection[j];
				if(n2->selected()){
					for(qsizetype k = j + 1; k < size; ++k){
						const auto n3 = mSelection[k];
						if(n3->selected()){
							if(!mGraph->triExists(n1, n2, n3, false)){
								mGraph->addTri(n1,n2,n3);
								dirty = true;
							}
						}
					}
				}
			}
		}
	}
	if(dirty){
		update();
	}
}

*/

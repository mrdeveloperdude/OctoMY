#include "Tri.hpp"


#include <QDataStream>
#include <QDebug>
#include <QVector2D>
#include <QtMath>

#include <vector>
#include <cmath>
#include <QtMath>


#include "Node.hpp"

Tri::Tri(QSharedPointer<Graph> graph, QSharedPointer<Node> n1, QSharedPointer<Node> n2, QSharedPointer<Node> n3)
	: mGraph(graph)
	, mN1(n1)
	, mN2(n2)
	, mN3(n3)
{

}


QSharedPointer<Graph> Tri::graph() const{
	return mGraph;
}

QSharedPointer<Node> Tri::node1() const{
	return mN1;
}

QSharedPointer<Node> Tri::node2() const{
	return mN2;
}

QSharedPointer<Node> Tri::node3() const{
	return mN3;
}


QString Tri::name() const{
	return QString("<%1, %2, %3>").arg(mN1.isNull()?"NIL":mN1->name()).arg(mN2.isNull()?"NIL":mN2->name()).arg(mN3.isNull()?"NIL":mN3->name());
}



// Function to calculate the angle at a corner
static double calculateCornerAngle(const QPointF &corner, const QPointF &adj1, const QPointF &adj2) {
	QPointF vec1 = adj1 - corner;
	QPointF vec2 = adj2 - corner;
	
	double dotProduct = vec1.x() * vec2.x() + vec1.y() * vec2.y();
	double magnitude1 = qSqrt(vec1.x() * vec1.x() + vec1.y() * vec1.y());
	double magnitude2 = qSqrt(vec2.x() * vec2.x() + vec2.y() * vec2.y());
	
	double cosTheta = dotProduct / (magnitude1 * magnitude2);
	return qRadiansToDegrees(qAcos(qBound(-1.0, cosTheta, 1.0))); // Clamp to avoid precision errors
}

// Function to inset a triangle based on corner angles
static QPolygonF insetTriangle(const QPointF &p1, const QPointF &p2, const QPointF &p3, double insetDistance) {
	QPolygonF result;
	
	// Calculate center of the triangle
	QPointF center = (p1 + p2 + p3) / 3.0;
	
	// Calculate angles at each corner
	double angle1 = calculateCornerAngle(p1, p2, p3);
	double angle2 = calculateCornerAngle(p2, p3, p1);
	double angle3 = calculateCornerAngle(p3, p1, p2);
	
	// Adjust inset for each corner based on its angle
	auto adjustInset = [&](const QPointF &corner, double angle) -> QPointF {
		double factor = insetDistance / qSin(qDegreesToRadians(angle / 2.0)); // Adjust based on corner angle
		QPointF direction = (center - corner);
		double length = qSqrt(direction.x() * direction.x() + direction.y() * direction.y());
		if (length == 0.0) return corner; // Avoid division by zero
		direction /= length; // Normalize direction
		return corner + direction * factor;
	};
	
	// Compute adjusted vertices
	result << adjustInset(p1, angle1)
		   << adjustInset(p2, angle2)
		   << adjustInset(p3, angle3);
	
	return result;
}


void Tri::draw(QPainter &painter, const Style &style){
	if(!mN1){
		qWarning()<<"No Tri->node1";
		return;
	}
	if(!mN2){
		qWarning()<<"No Tri->node2";
		return;
	}
	if(!mN3){
		qWarning()<<"No Tri->node3";
		return;
	}
	const bool selected = mN1->selected() && mN2->selected() && mN3->selected();
	painter.setPen( selected ? style.triOutlineSelectedPen : style.triOutlinePen);
	painter.setBrush( selected ? style.triFillSelected : style.triFill);
	const auto tri = insetTriangle(mN1->pos(), mN2->pos(), mN3->pos(), 3.0);
	painter.drawPolygon(tri);
}


QDataStream& operator<<(QDataStream& out, const Tri& obj) {
	out << obj.mN1->id();
	out << obj.mN2->id();
	out << obj.mN3->id();
	qDebug() << "Stream OUT Tri: " << obj.mN1->id() << ", " << obj.mN2->id() << ", " << obj.mN3->id();
	return out;
}


QDataStream& operator>>(QDataStream& in, Tri& obj) {
	return in;
}

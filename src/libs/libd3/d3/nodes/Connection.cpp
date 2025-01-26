#include "Connection.hpp"

#include <QDataStream>
#include <QDebug>
#include <QVector2D>

#include "Node.hpp"

Connection::Connection(QSharedPointer<Graph> graph, QSharedPointer<Node> from, QSharedPointer<Node> to)
	: mGraph(graph)
	, mFrom(from)
	, mTo(to)
{
	relax();
}


QSharedPointer<Graph> Connection::graph() const{
	return mGraph;
}

QSharedPointer<Node> Connection::from() const{
	return mFrom;
}
QSharedPointer<Node> Connection::to() const{
	return mTo;
}

void Connection::relax(){
	mRestLength = (QVector2D(mFrom->pos()) - QVector2D(mTo->pos())).length();
}


void Connection::setForce(const qreal force){
	mForce = force;
}


void Connection::taintLine(){
	mLine_invalid = true;
}

void Connection::unTaintLine(){
	mLine_invalid = false;
}

QString Connection::name() const{
	return QString("%1->%2").arg(mFrom->name()).arg(mTo->name());
}


void Connection::draw(QPainter &painter, const Style &style){
	if(!mFrom){
		qWarning()<<"No connection->from";
		return;
	}
	if(!mTo){
		qWarning()<<"No connection->to";
		return;
	}
	const bool selected = mFrom->selected() && mTo->selected();
	painter.setPen( selected ? style.connectionSelectedPen : style.connectionPen);
	painter.drawLine(mFrom->pos(), mTo->pos());
}


QDataStream& operator<<(QDataStream& out, const Connection& obj) {
	out << obj.mFrom->id();
	out << obj.mTo->id();
	qDebug() << "Stream OUT Connection: from=" << obj.mFrom->id() << ", to=" << obj.mTo->id();
	return out;
}

QDataStream& operator>>(QDataStream& in, Connection& obj) {
	Q_UNUSED(obj);
	return in;
}

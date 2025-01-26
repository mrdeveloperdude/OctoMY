#include "Connection.hpp"

#include <QDataStream>
#include <QDebug>
#include <QVector2D>
#include <QPainterPath>
#include <QColor>

#include "Node.hpp"
#include "Graph.hpp"

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

qreal Connection::realLength(){
	if(mFrom.isNull()){
		return 0.0;
	}
	if(mTo.isNull()){
		return 0.0;
	}
	return (QVector2D(mFrom->pos()) - QVector2D(mTo->pos())).length();
}

void Connection::relax(){
	mRestLength = realLength();
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

QString Connection::expression() const{
	return mExpression;
}


bool Connection::expressionChanged(){
	const auto was = mExpressionChanged;
	mExpressionChanged = false;
	return was;
}


void Connection::setExpression(const QString &expression){
	if(mExpression != expression){
		mExpression = expression;
		mExpressionChanged = true;
	}
}



QColor calculateStressColor(qreal stress) {
	QColor tensionColor(255, 100, 75);
	QColor neutralColor(255,255,255);
	QColor compressionColor(75, 100, 255);
	if (stress < 0.0) {
		qreal t = (stress + 1.0) / 1.0;
		int r = tensionColor.red() + t * (neutralColor.red() - tensionColor.red());
		int g = tensionColor.green() + t * (neutralColor.green() - tensionColor.green());
		int b = tensionColor.blue() + t * (neutralColor.blue() - tensionColor.blue());
		return QColor(r, g, b);
	} else {
		qreal t = stress;
		int r = neutralColor.red() + t * (compressionColor.red() - neutralColor.red());
		int g = neutralColor.green() + t * (compressionColor.green() - neutralColor.green());
		int b = neutralColor.blue() + t * (compressionColor.blue() - neutralColor.blue());
		return QColor(r, g, b);
	}
}



void Connection::draw(QPainter &painter, const Style &style) {
	if (!mFrom) {
		qWarning() << "No connection->from";
		return;
	}
	if (!mTo) {
		qWarning() << "No connection->to";
		return;
	}

	QVector2D direction = QVector2D(mTo->pos() - mFrom->pos());
	qreal actualLength = direction.length();
	qreal restLength = mRestLength;
	qreal stressLimit = 1.0;
	qreal raw = (actualLength - restLength) / restLength;
	qreal stress = qBound(-stressLimit, raw, +stressLimit);
	qreal stretchAmount=0.9;
	direction.normalize();
	QVector2D perpendicular(-direction.y(), direction.x());
	
	qreal baseThickness = 10.0 - stress * 2.0;
	qreal edgeThickness = baseThickness * (1.0 + stretchAmount * stress);
	qreal midThickness = baseThickness * (1.0 - stretchAmount * stress);
	
	QPointF p1 = mFrom->pos() + perpendicular.toPointF() * edgeThickness * 0.5;
	QPointF p2 = mFrom->pos() - perpendicular.toPointF() * edgeThickness * 0.5;
	
	QPointF p3 = mTo->pos() + perpendicular.toPointF() * edgeThickness * 0.5;
	QPointF p4 = mTo->pos() - perpendicular.toPointF() * edgeThickness * 0.5;
	
	QPointF mid1 = (mFrom->pos() + mTo->pos()) / 2 + perpendicular.toPointF() * midThickness * 0.5;
	QPointF mid2 = (mFrom->pos() + mTo->pos()) / 2 - perpendicular.toPointF() * midThickness * 0.5;
	
	QPainterPath path;
	path.moveTo(p1);
	path.quadTo(mid1, p3);
	path.lineTo(p4);
	path.quadTo(mid2, p2);
	path.closeSubpath();
	
	painter.setBrush(calculateStressColor(stress));
	painter.setPen(Qt::NoPen);
	painter.drawPath(path);
}




QDataStream& operator<<(QDataStream& out, const Connection& obj) {
	if(!obj.mFrom){
		qWarning()<<"No connection->from";
		return out;
	}
	if(!obj.mTo){
		qWarning()<<"No connection->to";
		return out;
	}
	out << obj.mFrom->id();
	out << obj.mTo->id();
	out << obj.mRestLength;
	out << obj.mStiffness;
	out << obj.mDamping;
	qDebug() << "Stream OUT Connection: from=" << obj.mFrom->id() << ", to=" << obj.mTo->id() << ", rest="<< obj.mRestLength << ", stiff="<< obj.mStiffness << ", damp="<< obj.mDamping;
	return out;
}

QDataStream& operator>>(QDataStream& in, Connection& obj) {
	QString from;
	in >> from;
	obj.mFrom = obj.mGraph->nodeByID(from);
	QString to;
	in >> to;
	obj.mTo = obj.mGraph->nodeByID(to);
	in >> obj.mRestLength;
	in >> obj.mStiffness;
	in >> obj.mDamping;
	qDebug() << "Stream IN Connection: from=" << from << ", to=" << to << ", rest="<< obj.mRestLength << ", stiff="<< obj.mStiffness << ", damp="<< obj.mDamping;
	return in;
}

#include "Node.hpp"

#include "UniqueID.hpp"
#include "Graph.hpp"
#include "Style.hpp"

#include <QDataStream>
#include <QImage>
#include <QMetaObject>
#include <QStaticText>
#include <QRegularExpression>


static int nodeID=0;

static const QRegularExpression trailingDigitsRe("\\d+$");


Node::Node(QSharedPointer<Graph> graph, const QString &name, QPointF pos)
	: mGraph(graph)
	, mName(name)
	, mUniqueID(generateUniqueID())
	, mID(nodeID++)
	, mPos(pos)
{
}



void Node::draw(QPainter &painter, const bool xray, const Style &style){
	Q_UNUSED(xray);
	const auto r = rect();
	auto fill = mSelected ? style.nodeFillSelected : style.nodeFill;
	auto outline = mSelected ? style.nodeOutlineSelected : style.nodeOutline;
	if(mDebug){
		qDebug()<<"draw node"<<r <<fill << outline;
	}
	painter.setBrush(fill);
	painter.fillRect(r, fill);
	painter.setPen(outline);
	painter.drawRect(r);
	
	
	QRect fr = painter.fontMetrics().boundingRect(mName);
	auto s = r.size();
	auto p = r.center().toPoint();
	fr.moveCenter(p);
	painter.setPen(outline);
	painter.drawText(fr, Qt::AlignCenter, mName);
}




void Node::taint(const QString &reason){
	if(!mGraph.isNull()){
		mGraph->taint(reason);
	}
}




QString Node::toString() const{
	QString out("node");
	return out;
}


QDataStream& operator<<(QDataStream& out, const Node& obj) {
	out << obj.mName;
	out << obj.mUniqueID;
	out << obj.mPos;
	if(obj.mDebug){
		qDebug() << "Stream OUT TrackerNode: name=" << obj.mName << ", uniqueID=" << obj.id() << ", pos=" << obj.mPos;
	}
	return out;
}


QDataStream& operator>>(QDataStream& in, Node& obj) {
	in >> obj.mName;
	in >> obj.mUniqueID;
	in >> obj.mPos;
	if(obj.mDebug){
		qDebug() << "Stream IN TrackerNode: name=" << obj.mName << ", uniqueID=" << obj.id() << ", pos=" << obj.mPos;
	}
	return in;
}

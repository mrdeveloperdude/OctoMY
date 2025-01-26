#include "Node.hpp"

#include "Graph.hpp"
#include "Project.hpp"
#include "Style.hpp"

#include <QDataStream>
#include <QImage>
#include <QMetaObject>
#include <QStaticText>
#include <QRegularExpression>


static const QRegularExpression trailingDigitsRe("\\d+$");


Node::Node(QSharedPointer<Project> project, const QString &name, QPointF pos)
	: mProject(project)
	, mName(name)
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
	painter.setPen(outline);
	if(mAnchored){
		painter.fillRect(r, fill);
		painter.drawRect(r);
	}
	else{
		painter.drawEllipse(r);
	}
	QRect fr = painter.fontMetrics().boundingRect(mName);
	//auto s = r.size();
	auto p = r.center().toPoint();
	fr.moveCenter(p);
	painter.setPen(outline);
	painter.drawText(fr, Qt::AlignCenter, mName);
}


void Node::taint(const QString &reason){
	if(!mProject.isNull()){
		mProject->taint(reason);
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
	out << obj.mSelected;
	out << obj.mAnchored;
	out << obj.mMass;
	if(obj.mDebug){
		qDebug() << "Stream OUT TrackerNode: name=" << obj.mName << ", uniqueID=" << obj.id() << ", pos=" << obj.mPos << ", sel=" << obj.mSelected << ", anchored=" << obj.mAnchored << ", mass=" << obj.mMass;
	}
	return out;
}


QDataStream& operator>>(QDataStream& in, Node& obj) {
	in >> obj.mName;
	in >> obj.mUniqueID;
	in >> obj.mPos;
	in >> obj.mSelected;
	in >> obj.mAnchored;
	in >> obj.mMass;
	if(obj.mDebug){
		qDebug() << "Stream IN TrackerNode: name=" << obj.mName << ", uniqueID=" << obj.id() << ", pos=" << obj.mPos << ", sel=" << obj.mSelected << ", anchored=" << obj.mAnchored << ", mass=" << obj.mMass;
	}
	return in;
}

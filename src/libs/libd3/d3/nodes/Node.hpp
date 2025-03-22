#ifndef NODE_H
#define NODE_H

#include "Style.hpp"

#include "UniqueID.hpp"


#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QPointF>
#include <QVector3D>
#include <QRectF>
#include "uptime/SharedPointerWrapper.hpp"
#include <QSizeF>
#include <QString>

class Graph;
class Project;

class Node{
private:
	QSharedPointer<Project> mProject;
	QString mName;
	UniqueID mUniqueID;
	bool mDebug{false};
	bool mSelected{false};
	
private:
	bool mAnchored{false};
	QVector3D mPos;
	QVector3D mVelocity;
	QVector3D mForce;
	qreal mMass{1.0};

private:
	QSizeF mSize{30, 30};

public:
	Node() = delete;
	Node(QSharedPointer<Project> project=nullptr, const QString &name=QString(), QPointF pos=QPoint());

private:
	void taint(const QString &reason);

public:
	QVector3D velocity() const { return mVelocity; }
	void applyForce(const QVector3D &force) {
		mForce += force;
	}
	void integrate(qreal dt) {
		auto acceleration = mForce / mMass;
		mVelocity += acceleration * dt;
		if(!mAnchored){
			mPos += mVelocity * dt;
		}
		mForce = QVector3D();
	}

public:
	QString toString() const;
	inline void setName(const QString &name);
	inline QString name() const;
	inline int idInt() const{
		return mUniqueID.id();
	}
	inline QString id() const{
		return mUniqueID.uniqueID();
	}
	
	inline QPointF pos() const;
	inline qreal mass() const;
	inline void setMass(qreal mass);
	inline QVector3D pos3d() const;
	inline QRectF rect() const;
	inline QSizeF size() const;
	inline void setSize(QSizeF size);
	inline void setPos(QPointF pos);
	
	inline bool isSelected() const;
	inline void setSelected(bool selected);
	
	inline bool isAnchored() const;
	inline void setAnchored(bool anchored);
	
	inline void deSelect();
	inline void select();
	
	void draw(QPainter &painter, const bool xray, const Style &style);

public:
	friend QDataStream& operator<<(QDataStream& out, const Node& obj);
	friend QDataStream& operator>>(QDataStream& in, Node& obj);

};


void Node::setName(const QString &name) {
	if(mName != name){
		mName = name;
		taint("setName");
	}
}


QString Node::name() const{
	return mName;
}



QPointF Node::pos() const{
	return QPointF(mPos.x(), mPos.y());
}

qreal Node::mass() const{
	return mMass;
}

void Node::setMass(qreal mass){
	mMass= mass;
}


QVector3D Node::pos3d() const{
	return mPos;
}


QSizeF Node::size() const{
	return mSize;
}


void Node::setSize(QSizeF size){
	mSize = size;
}


QRectF Node::rect() const{
	auto rect = QRectF (pos(), mSize).translated(-mSize.width()/2, -mSize.height()/2);
	return rect;
}

void Node::setPos(QPointF pos) {
	//qDebug() << "POS" << mName << " " << mPos << " -> " << pos;
	auto opos = this->pos();
	if(opos != pos){
		mPos.setX(pos.x());
		mPos.setY(pos.y());
		mPos.setZ(0.0);
		taint("setPos");
	}
}


bool Node::isSelected() const{
	return mSelected;
}


void Node::setSelected(bool selected){
	mSelected = selected;
}



bool Node::isAnchored() const{
	return mAnchored;
}


void Node::setAnchored(bool anchored){
	mAnchored = anchored;
}


void Node::deSelect(){
	setSelected(false);
}


void Node::select(){
	setSelected(true);
}



#endif // NODE_H

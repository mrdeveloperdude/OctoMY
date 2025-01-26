#ifndef NODE_H
#define NODE_H

#include "Style.hpp"

#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QPointF>
#include <QRectF>
#include <QSharedPointer>
#include <QSizeF>
#include <QString>

class Graph;

class Node{
private:
	QSharedPointer<Graph> mGraph;
	QString mName;
	QString mUniqueID;
	int mID;
	bool mDebug{false};
	bool mSelected{false};

private:
	QPointF mPos;
	QSizeF mSize{30, 30};
	
public:
	Node() = delete;
	Node(QSharedPointer<Graph> graph, const QString &name, QPointF pos=QPoint());
	
private:
	
	void taint(const QString &reason);
	
	
public:
	QString toString() const;
	inline void setName(const QString &name);
	inline QString name() const;
	inline int idInt() const;
	inline QString id() const;
	inline QPointF pos() const;
	inline QRectF rect() const;
	inline QSizeF size() const;
	inline void setSize(QSizeF size);
	inline void setPos(QPointF pos);
	
	inline bool selected() const;
	inline void setSelected(bool selected);
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


QString Node::id() const{
	return mUniqueID;
}


int Node::idInt() const{
	return mID;
}


QPointF Node::pos() const{
	return mPos;
}


QSizeF Node::size() const{
	return mSize;
}


void Node::setSize(QSizeF size){
	mSize = size;
}


QRectF Node::rect() const{
	auto rect = QRectF (mPos, mSize).translated(-mSize.width()/2, -mSize.height()/2);
	return rect;
}

void Node::setPos(QPointF pos) {
	//qDebug() << "POS" << mName << " " << mPos << " -> " << pos;
	if(mPos != pos){
		mPos = pos;
		taint("setPos");
	}
}


bool Node::selected() const{
	return mSelected;
}


void Node::setSelected(bool selected){
	mSelected = selected;
}


void Node::deSelect(){
	setSelected(false);
}


void Node::select(){
	setSelected(true);
}



#endif // NODE_H

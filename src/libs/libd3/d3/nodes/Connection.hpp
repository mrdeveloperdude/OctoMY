#ifndef CONNECTION_H
#define CONNECTION_H


#include "UniqueID.hpp"


#include <QDataStream>
#include "uptime/SharedPointerWrapper.hpp"

class Graph;
class Node;
class QPainter;
struct Style;

class Connection{
private:
	const UniqueID mUniqueID;
	QSharedPointer<Graph> mGraph;
	QSharedPointer<Node> mFrom;
	QSharedPointer<Node> mTo;
	qreal mRestLength{0.0};
	qreal mStiffness{100.0};
	qreal mDamping{0.1};
	qreal mForce{0.0};
	bool mLine_invalid{true};
	QString mExpression;
	bool mExpressionChanged{false};
	
public:
	Connection() = default;
	Connection(QSharedPointer<Graph> graph=nullptr, QSharedPointer<Node> from=nullptr, QSharedPointer<Node> to=nullptr);

public:
	
	inline int idInt() const{
		return mUniqueID.id();
	}
	inline QString id() const{
		return mUniqueID.uniqueID();
	}
	
	QSharedPointer<Graph> graph() const;
	QSharedPointer<Node> from() const;
	QSharedPointer<Node> to() const;
	
	qreal restLength(){return mRestLength;}
	qreal stiffness(){return mStiffness;}
	qreal damping(){return mDamping;}
	qreal realLength();
	void relax();
	void setForce(const qreal force);
	
	void taintLine();
	void unTaintLine();
	
	QString name() const;
	QString expression() const;
	bool expressionChanged();
	void setExpression(const QString &expression);

public:
	void draw(QPainter &painter, const Style &style);

public:
	friend QDataStream& operator<<(QDataStream& out, const Connection& obj);
	friend QDataStream& operator>>(QDataStream& in, Connection& obj);

};
#endif // CONNECTION_H

#ifndef PROJECT_HPP
#define PROJECT_HPP

#include "Graph.hpp"
#include "Style.hpp"
#include "ExpressionsRegistry.hpp"

#include <QSharedPointer>
#include <QObject>

class Player;

class Project: public QObject
{
	Q_OBJECT;
private:
	QSharedPointer<Graph> mGraph;
	QSharedPointer<Style> mStyle;
	QSharedPointer<Player> mPlayer;
	QSharedPointer<ExpressionsRegistry> mExpressions;
	bool mTainted{false};
	QString mLastTaintReason;
	
public:
	Project();
	
public:
	void stepExpressions(qreal dt);
	void step(qreal dt);
	QSharedPointer<Graph> graph();
	QSharedPointer<Style> style();
	QSharedPointer<Player> player();
	void setStyle(QSharedPointer<Style> style);
	void setGraph(QSharedPointer<Graph> graph);
	void setPlayer(QSharedPointer<Player> player);
	
	void taint(const QString &reason);
	void unTaint(const QString &reason);
	bool isTainted() const;
	
	friend bool saveProject(QSharedPointer<Project> project, const QString &filename, const bool compress);
	friend bool loadProject(QSharedPointer<Project> project, const QString &filename);

};

QSharedPointer<Project> createProject();



bool saveProject(QSharedPointer<Project> project, const QString &filename, const bool compress=true);

bool loadProject(QSharedPointer<Project> project, const QString &filename);

#endif // PROJECT_HPP

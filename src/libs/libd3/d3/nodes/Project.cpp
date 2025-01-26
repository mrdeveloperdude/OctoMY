#include "Project.hpp"

#include "Graph.hpp"
#include "Player.hpp"


#include <QDebug>
#include <QIODevice>
#include <QFile>
#include <QTimer>

static const QString MAGIC{"NOVI"};
static const quint32 VERSION{1};


Project::Project()
	: mStyle(new Style())
{

}


QSharedPointer<Project> createProject(){
	auto project = QSharedPointer<Project>::create();
	project->setGraph(QSharedPointer<Graph>::create(project));
	project->setPlayer(QSharedPointer<Player>::create(project));
	return project;
}

QSharedPointer<Style> Project::style(){
	return mStyle;
}

QSharedPointer<Graph> Project::graph(){
	return mGraph;
}


void Project::setGraph(QSharedPointer<Graph> graph){
	mGraph=graph;
}



void Project::setStyle(QSharedPointer<Style> style){
	mStyle=style;
}

QSharedPointer<Player> Project::player(){
	return mPlayer;
}


void Project::setPlayer(QSharedPointer<Player> player){
	mPlayer = player;
}


void Project::stepExpressions(qreal dt){
	if(!mExpressions.isNull()){
		mExpressions->step(dt);
	}
}

void Project::step(qreal dt){
	qDebug()<<"step"<<dt*1000.0<<"ms";
	//mAnimator->step(dt);
	stepExpressions(dt);
	mGraph->step(dt);
}



bool saveProject(QSharedPointer<Project> project, const QString &filename, const bool compress){
	qDebug() << "Saving "<<filename;
	if(!project){
		qWarning()<<"No project to save, skipping...";
		return false;
	}
	auto graph = project->graph();
	if(!graph){
		qWarning()<<"No graph to save, skipping...";
		return false;
	}
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) {
		qWarning()<<"Could not open file for saving, skipping...";
		return false;
	}
	QDataStream out(&file);
	out.writeRawData(MAGIC.toUtf8().constData(), 4);
	out << VERSION;
	out << compress;
	if (compress) {
		QByteArray data;
		QDataStream dataStream(&data, QIODevice::WriteOnly);
		dataStream << *graph;
		QByteArray compressedData = qCompress(data);
		out << compressedData;
	} else {
		out << *graph;
	}
	file.close();
	project->unTaint("saveGraph");
	return true;
}


bool loadProject(QSharedPointer<Project> project, const QString &filename){
	qDebug() << "Loading "<<filename;
	if(!project){
		qWarning()<<"No project to save, skipping...";
		return false;
	}
	auto graph = project->graph();
	if(!graph){
		qWarning()<<"No graph to load, skipping...";
		return false;
	}
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		qWarning()<<"Could not open file for loading, skipping...";
		return false;
	}
	QDataStream in(&file);
	char magic[5] = {0};
	in.readRawData(magic, 4);
	if (QString::fromUtf8(magic) != MAGIC) {
		file.close();
		qWarning()<<"Wrong MAGIC "<< QString::fromUtf8(magic) << " found in file while loading, skipping...";
		return false;
	}
	quint32 version;
	in >> version;
	if (version != VERSION) {
		file.close();
		qWarning()<<"Wrong VERSION found in file while loading, skipping...";
		return false;
	}
	bool compress{false};
	in >> compress;
	if (compress) {
		QByteArray compressedData;
		in >> compressedData; 
		QByteArray data = qUncompress(compressedData);
		QDataStream dataStream(&data, QIODevice::ReadOnly);
		dataStream >> *graph;
	} else {
		in >> *graph;
	}
	project->unTaint("loadGraph");
	return true;
}



void Project::taint(const QString &reason){
	if(mLastTaintReason != reason){
		qDebug() << "Project tainted by" << reason;
		mLastTaintReason = reason;
	}
	mTainted = true;
}


void Project::unTaint(const QString &reason){
	qDebug()<<"Project un-tainted by"<<reason;
	mTainted = false;
}


bool Project::isTainted() const{
	return mTainted;
}


#include "Scene.hpp"

#include <QDebug>

#include "nodes/Layer.hpp"
#include "RhiContext.hpp"
#include "Camera.hpp"

Scene::Scene()
	: mCamera( QSharedPointer<Camera>::create() )
{
	//qDebug() << "Scene::ctor(nodes=" << mNodes.size() << ")";
	mCamera->setPerspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
	mCamera->setPosition(QVector3D(0.0f, 2.0f, 5.0f));
	mCamera->setTarget(QVector3D(0.0f, 0.0f, 0.0f));
}

QSharedPointer<Camera> Scene::camera(){
	return mCamera;
}

void Scene::initialize(RhiContext &context){
	//qDebug() << "Scene::initialize(" << mNodes.size() << ")";
	for (auto &node : mNodes) {
		node->initialize(context);
	}
}


void Scene::update(RhiContext &context, qint64 interval){
	//qDebug() << "Scene::update(nodes=" << mNodes.size() << ")";
	//mCamera->orbit(0.5f, 0.123f);
	for (auto &node : mNodes) {
		node->update(context, interval);
	}
}


void Scene::render(RhiContext &context){
	//qDebug() << "Scene::render(nodes=" << mNodes.size() << ")";
	for (auto &node : mNodes) {
		node->render(context);
	}
}


void Scene::releaseResources(){
	//qDebug() << "Scene::releaseResources(nodes=" << mNodes.size() << ")";
	for (auto &node : mNodes) {
		node->releaseResources();
	}
}


void Scene::clear(){
	//qDebug() << "Scene::clear(nodes=" << mNodes.size() << ")";
	releaseResources();
	mNodes.clear();
}

#include "nodes/CubeNode.hpp"

#include <QRandomGenerator>
void Scene::addCube(RhiContext &context)
{
	//qDebug() << "Scene::addCube(nodes=" << mNodes.size() << )";
	auto node = std::make_unique<CubeNode>(sharedFromThis());
	node->initialize(context);
	auto mat = QMatrix4x4();
	mat.translate(QRandomGenerator::system()->generateDouble(), QRandomGenerator::system()->generateDouble(), QRandomGenerator::system()->generateDouble());
	node->setTransform(mat);
	mNodes.push_back(std::move(node));
}


#include "nodes/TriNode.hpp"

void Scene::addTri(RhiContext &context)
{
	qDebug() << "Scene::addTri(nodes=" << mNodes.size() << ")";
	auto node = std::make_unique<TriNode>(sharedFromThis());
	node->initialize(context);
	mNodes.push_back(std::move(node));
}
#include "nodes/TexTriNode.hpp"

void Scene::addTexTri(RhiContext &context)
{
	qDebug() << "Scene::addTexTri(nodes=" << mNodes.size() << ")";
	auto node = std::make_unique<TexTriNode>(sharedFromThis());
	node->initialize(context);
	mNodes.push_back(std::move(node));
}

#include "nodes/GroundPlane.hpp"

void Scene::addGroundplane(RhiContext &context)
{
	qDebug() << "Scene::addGroundplane(nodes=" << mNodes.size() << ")";
	auto node = std::make_unique<GroundPlane>(sharedFromThis());
	node->initialize(context);
	mNodes.push_back(std::move(node));
}



#include "nodes/ShapeNode.hpp"

void Scene::addShape(RhiContext &context)
{
	qDebug() << "Scene::addShape(nodes=" << mNodes.size() << ")";
	auto node = std::make_unique<ShapeNode>(sharedFromThis());
	node->initialize(context);
	mNodes.push_back(std::move(node));
}

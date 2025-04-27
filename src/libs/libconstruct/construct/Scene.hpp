#ifndef SCENE_H
#define SCENE_H

#include "Camera.hpp"

#include <vector>

#include <QColor>
#include <QSharedPointer>

class Layer;
class RhiContext;

class Scene : public QEnableSharedFromThis<Scene> 
{
private:
	std::vector<std::unique_ptr<Layer>> mNodes;
	QColor mClearColorA{QColor::fromRgbF(0.0, 0.4, 0.7, 1.0)};
	QColor mClearColorB{QColor::fromRgbF(0.7, 0.4, 0.0, 1.0)};
	bool mFlip{false};
	QSharedPointer<Camera> mCamera;

public:
	Scene();

public:
	// Gain access to the camera
	QSharedPointer<Camera> camera();

public:
	// RHI interface extended for all nodes in graph
	void initialize(RhiContext &context);
	void update(RhiContext &context, qint64 interval);
	void render(RhiContext &context);
	void releaseResources();

public:
	// Remove all nodes in graph
	void clear();

	// Add various node types
	void addCube(RhiContext &context);
	void addTri(RhiContext &context);
	void addTexTri(RhiContext &context);
	void addGroundplane(RhiContext &context);
	void addShape(RhiContext &context);
	// Return current background clear color (alternating)
	QColor clearColor(){ mFlip=!mFlip; return mFlip?mClearColorA:mClearColorB; }
};

#endif // SCENE_H
